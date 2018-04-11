#pragma once
#include "promise.h"
#include <thread>
namespace a_promise_namespace {
    template< class Function, class... Args>
    promise<typename std::result_of<Function(Args...)>::type>
        async_call(Function&& f, Args&&... args) {
        auto _cb = std::bind(std::forward<Function>(f), std::forward<Args>(args)...);
        return promise<typename std::result_of<Function(Args...)>::type>(std::async(std::launch::async, _cb));
    }
}
