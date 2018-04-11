#pragma once
#include <future>
#include <thread>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <mutex>
#include <type_traits>
#include <condition_variable>
namespace a_promise_namespace {
    template<typename T>
    class promise
    {
      public:
        promise() {}
        promise(std::future<T>&& val) :_future(std::forward<std::future<T>&&>(val)) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
        }
        promise(promise&& val):_future(std::forward<std::future<T>&& >(val._future)) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
        }
        promise(const promise&) = delete;
        promise& operator=(const promise&) = delete;
        promise& operator=(promise&& val) {
            if (&val != this) {
                _future = std::move(val._future);
                if (!_future.valid())
                    throw std::runtime_error("bad future");
            }
        }
        promise& operator=(std::future<T>&& val) {
            _future = std::forward<std::future<T>&& >(val);
            if (!_future.valid())
                throw std::runtime_error("bad future");
            return *this;
        }
        ~promise() {
        }
        T get() {
            std::unique_lock<std::mutex> locker(_mut);
            return _future.get();
        }
        bool is_valid() {
            std::unique_lock<std::mutex> locker(_mut);
            return _future.valid();
        }
        template < class F, class... Args>
        auto than(F&& f, Args&&... args) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
            std::condition_variable _cv;
            promise<typename std::result_of<F(decltype(_future.get()), Args...)>::type> new_prom(std::async(std::launch::async, [&]()->typename std::result_of < F(decltype(_future.get()), Args...)>::type {
                std::unique_lock<std::mutex> locker(_mut);
                auto _fut = std::move(_future);
                locker.unlock();
                _cv.notify_one();
                auto _ret = _fut.get();
                auto _cb = std::bind(std::forward<F>(f), _ret, std::forward<Args>(args)...);
                return _cb();
            }));
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker);
            return new_prom;
        }
      private:
        std::future<T> _future;
        std::mutex _mut;
    };

    template<>
    class promise<void>
    {
    public:
        promise() {}
        promise(std::future<void>&& val) :_future(std::forward<std::future<void> >(val)) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
        }
        promise(promise&& val):_future(std::forward<std::future<void> >(val._future)) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
        }
        ~promise() {
        
        }
        promise(const promise&) = delete;
        promise& operator=(const promise&) = delete;
        promise& operator=(promise&& val) {
            if (&val != this) {
                _future.operator=(std::forward<std::future<void> >(val._future));
                if (!_future.valid())
                    throw std::runtime_error("bad future");
            }
        }
        promise& operator=(std::future<void>&& val) {
            _future.operator=(std::forward<std::future<void> >(val));
            if (!_future.valid())
                throw std::runtime_error("bad future");
            return *this;
        }
        void get() {
          std::unique_lock<std::mutex> locker(_mut);
          _future.get();
        }
        bool is_valid() {
            std::unique_lock<std::mutex> locker(_mut);
            return _future.valid();
        }
        template < class F, class... Args>
        auto than(F&& f, Args&&... args) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
            std::condition_variable _cv;
            promise<typename std::result_of < F(Args...)>::type> _ret_prom(async_call( [&]()->typename std::result_of < F( Args...)>::type {
                std::unique_lock<std::mutex> locker(_mut);
                auto _fut = std::move(_future);
                locker.unlock();
                _cv.notify_one();
                _fut.get();
                auto _cb = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
                return _cb();
            }));
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker);
            return _ret_prom;
        }
    private:
        std::future<void> _future;
        std::mutex _mut;
    };  
}

