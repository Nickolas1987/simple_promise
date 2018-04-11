#include <promise.h>
#include <async_call.h>
#include <gtest/gtest.h>
#include <thread>
#include <iostream>
class TestLog : public ::testing::Test
{
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

TEST_F(TestLog, test1)
{
     auto foo = [](int val)->int{
        return val*10;
     };
     auto bar = [](int in, int add)->int{
        return in + add;
     };
    auto _prom = a_promise_namespace::async_call(foo,5).then(bar, 7).then([](int res) {
		std::cout << "result " << res << std::endl; 
		throw std::runtime_error("test exception");
    });

    std::cout << "other action" << std::endl;
    try{
      _prom.get();
    }
    catch(std::exception& _e){
      std::cout << _e.what() << std::endl;
    }
}


