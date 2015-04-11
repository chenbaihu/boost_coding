#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

int g_value = 0;
boost::mutex io_mutex;

struct count
{
    count(int id) : id(id) { }

    void operator()()
    {
        for (int i = 0; i < 10; ++i)
        {
            boost::mutex::scoped_lock lock(io_mutex);
            std::cout << id << ": " << i << std::endl;
            g_value++;
        }
    }

    int id;
};

int main(int argc, char* argv[])
{
    boost::thread thrd1(count(1));
    boost::thread thrd2(count(2));

    thrd1.join();
    thrd2.join();

    std::cout << "g_value=" << g_value << std::endl;
    return 0;
}

