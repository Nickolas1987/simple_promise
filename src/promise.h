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
        promise(std::future<T>&& val) :_future(std::forward<std::future<T>&&>(val)), _notified(false) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
        }
        promise(promise&& val):_future(std::forward<std::future<T>&& >(val._future)), _notified(false) {
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
        void finish() {
            std::condition_variable _cv;
            std::thread _th([&]() {
                try {
                    std::unique_lock<std::mutex> locker(_mut);
                    auto _fut = std::move(_future);
                    _notified = true;
                    locker.unlock();
                    _cv.notify_one();
                    _fut.get();
                }
                catch (...) {
                }
            });
            _th.detach();
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker, [&] { return _notified; });
        }
        template < class F>
        auto then(F&& f) {
            std::condition_variable _cv;
            promise<typename std::result_of<F(T)>::type> new_prom(std::async(std::launch::async, [&]()->typename std::result_of < F(T)>::type {
                std::unique_lock<std::mutex> locker(_mut);
                auto _fut = std::move(_future);
                _notified = true;
                locker.unlock();
                _cv.notify_one();
                auto _ret = _fut.get();
                auto _cb = std::bind(std::forward<F>(f), _ret);
                return _cb();
            }));
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker, [&] { return _notified; });
            return new_prom;
        }
        template < class F, class E>
        auto then(F&& f, E&& ef) {
            std::condition_variable _cv;
            promise<typename std::result_of<F(T)>::type> new_prom(std::async(std::launch::async, [&]()->typename std::result_of < F(T)>::type {
                std::unique_lock<std::mutex> locker(_mut);
                auto _fut = std::move(_future);
                _notified = true;
                locker.unlock();
                _cv.notify_one();
                try {
                    auto _ret = _fut.get();
                    auto _cb = std::bind(std::forward<F>(f), _ret);
                    return _cb();
                }
                catch (...) {
                    std::exception_ptr _eptr = std::current_exception();
                    auto _eb = std::bind(std::forward<E>(ef), _eptr);
                    _eb();
                    std::rethrow_exception(_eptr);
                }
            }));
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker, [&]() {return _notified; });
            return new_prom;
        }
      private:
        std::future<T> _future;
        std::mutex _mut;
        bool _notified;
    };

    template<>
    class promise<void>
    {
    public:
        promise():_notified(false) {}
        promise(std::future<void>&& val) :_future(std::forward<std::future<void> >(val)),_notified(false) {
            if (!_future.valid())
                throw std::runtime_error("bad future");
        }
        promise(promise&& val):_future(std::forward<std::future<void> >(val._future)), _notified(false) {
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
        void finish() {
            std::condition_variable _cv;
            std::thread _th([&]() {
                try {
                    std::unique_lock<std::mutex> locker(_mut);
                    auto _fut = std::move(_future);
                    _notified = true;
                    locker.unlock();
                    _cv.notify_one();
                    _fut.get();
                }
                catch (...) {
                }
            });
            _th.detach();
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker, [&] { return _notified; });
        }
        template < class F>
        auto then(F&& f) {
            std::condition_variable _cv;
            promise<typename std::result_of < F()>::type> _ret_prom(async_call( [&]()-> typename std::result_of < F()>::type {
                std::unique_lock<std::mutex> locker(_mut);
                auto _fut = std::move(_future);
                _notified = true;
                locker.unlock();
                _cv.notify_one();
                _fut.get();
                auto _cb = std::bind(std::forward<F>(f));
                return _cb();
            }));
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker, [&] { return _notified; });
            return _ret_prom;
        }
        template < class F, class E>
        auto then(F&& f, E&& ef) {
            std::condition_variable _cv;
            promise<typename std::result_of<F()>::type> new_prom(std::async(std::launch::async, [&]()-> typename std::result_of < F()>::type {
                std::unique_lock<std::mutex> locker(_mut);
                auto _fut = std::move(_future);
                _notified = true;
                locker.unlock();
                _cv.notify_one();
                try {
                    _fut.get();
                    auto _cb = std::bind(std::forward<F>(f));
                    return _cb();
                }
                catch (...) {
                    std::exception_ptr _eptr = std::current_exception();
                    auto _eb = std::bind(std::forward<E>(ef), _eptr);
                    _eb();
                    std::rethrow_exception(_eptr);
                }
            }));
            std::unique_lock<std::mutex> locker(_mut);
            _cv.wait(locker, [&] { return _notified; });
            return new_prom;
        }
    private:
        std::future<void> _future;
        std::mutex _mut;
        bool _notified;
    };  
}

