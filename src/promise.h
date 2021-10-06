#pragma once
#include <future>
#include <thread>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <mutex>
#include <type_traits>
#include <condition_variable>
namespace a_promise_namespace 
{
    template<typename T>
    class promise
    {
    public:
        promise() {}
        promise(std::future<T>&& val) noexcept : m_future(std::forward<std::future<T>>(val)) 
        {
        }
        promise(promise&& val) noexcept : m_future(std::forward<std::future<T>>(val.m_future))
        {
        }
        promise(const promise&) noexcept = delete;
        promise& operator=(const promise&) = delete;
        promise& operator=(promise&& val) noexcept
        {
            if (&val != this) 
            {
                m_future = std::move(val.m_future);
            }
        }
        promise& operator=(std::future<T>&& val) 
        {
            m_future = std::forward<std::future<T>&& >(val);
            if (!m_future.valid())
            {
                throw std::runtime_error("bad future");
            }
            return *this;
        }
        ~promise() 
        {
        }
        void finish() 
        {
            std::thread th([localFut = std::move(m_future)]() mutable 
            {
                try 
                {
                    localFut.get();
                }
                catch (...) 
                {
                }
            });
            th.detach();
        }
        template <class F>
        auto then(F&& f) 
        {
            promise<typename std::result_of<F(T)>::type> newProm(std::async(std::launch::async, [localFut = std::move(m_future), localFunc = f]() mutable -> typename std::result_of < F(T)>::type
            {
                auto ret = localFut.get();
                auto cb = std::bind(std::forward<F>(localFunc), ret);
                return cb();
            }));
            return newProm;
        }
        template < class F, class E>
        auto then(F&& f, E&& ef) 
        {
            promise<typename std::result_of<F(T)>::type> newProm(std::async(std::launch::async, [localFut = std::move(m_future), localFunc = f, localErrFunc = ef]() mutable -> typename std::result_of < F(T)>::type 
            {
                try 
                {
                    auto ret = localFut.get();
                    auto cb = std::bind(std::forward<F>(localFunc), ret);
                    return cb();
                }
                catch (...) 
                {
                    std::exception_ptr eptr = std::current_exception();
                    auto eb = std::bind(std::forward<E>(localErrFunc), eptr);
                    eb();
                    std::rethrow_exception(eptr);
                }
            }));
            return newProm;
        }
    private:
        std::future<T> m_future;
    };

    template<>
    class promise<void>
    {
    public:
        promise() {}
        promise(std::future<void>&& val) :m_future(std::forward<std::future<void> >(val)) 
        {
            if (!m_future.valid())
            {
                throw std::runtime_error("bad future");
            }
        }
        promise(promise&& val) noexcept : m_future(std::forward<std::future<void> >(val.m_future))
        {
        }
        ~promise() 
        {
        }
        promise(const promise&) = delete;
        promise& operator=(const promise&) noexcept = delete;
        promise& operator=(promise&& val) noexcept
        {
            if (&val != this) 
            {
                m_future.operator=(std::forward<std::future<void> >(val.m_future));
            }
            return *this;
        }
        promise& operator=(std::future<void>&& val) noexcept
        {
            m_future.operator=(std::forward<std::future<void> >(val));
            return *this;
        }
        void finish() 
        {
            std::thread th([localFut = std::move(m_future)]() mutable
            {
                try
                {
                    localFut.get();
                }
                catch (...)
                {
                }
            });
            th.detach();
        }
        template < class F>
        auto then(F&& f) 
        {
            promise<typename std::result_of < F()>::type> newProm(async_call([localFut = std::move(m_future), localFunc = f]() mutable -> typename std::result_of < F()>::type
            {
                localFut.get();
                auto cb = std::bind(std::forward<F>(localFunc));
                return cb();
            }));
            return newProm;
        }
        template < class F, class E>
        auto then(F&& f, E&& ef) {
            promise<typename std::result_of<F()>::type> newProm(std::async(std::launch::async, [localFut = std::move(m_future), localFunc = f, localErrFunc = ef]() mutable -> typename std::result_of < F()>::type
            {
                try
                {
                    localFut.get();
                    auto cb = std::bind(std::forward<F>(localFunc));
                    return cb();
                }
                catch (...) 
                {
                    std::exception_ptr eptr = std::current_exception();
                    auto eb = std::bind(std::forward<E>(localErrFunc), eptr);
                    eb();
                    std::rethrow_exception(eptr);
                }
            }));
            return newProm;
        }
    private:
        std::future<void> m_future;
    };
}
