#include <promise.h>
#include <async_call.h>
#include <gtest/gtest.h>
#include <thread>
#include <iostream>
#include <chrono>
using namespace std::chrono_literals;
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
        std::cout << "foo" <<std::endl;
        return val*10;
     };
     auto bar = [](int in, int add)->int{
        std::this_thread::sleep_for(5s);
        std::cout << "bar" <<std::endl;
        return in + add;
     };
     a_promise_namespace::async_call([&]()->int{
                return foo(10);
     }).then([&](int val)->int {
                return bar(val,7);
     }).then([](int res) {
                std::cout << " result " << res << std::endl; 
                throw std::runtime_error("test exception");
     }).then([]() {}, [](const std::exception_ptr& e) {
                try {
                  std::rethrow_exception(e);
                }
                catch (std::runtime_error& ex) {
                  std::cout << ex.what() << std::endl;
                }
                catch (...) {
                }
     }).finish();
     
     std::cout << "other action" << std::endl;
     std::cin.get();
}


