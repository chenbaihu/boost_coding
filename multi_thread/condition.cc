#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <iostream>

const int BUF_SIZE = 10;
const int ITERS = 100;

boost::mutex io_mutex;   // 仅仅是为了让输出日志不错乱

class buffer
{
    public:
        typedef boost::mutex::scoped_lock scoped_lock;

        buffer() : p_(0), c_(0), full_(0) {
        }

        void put(int m)
        {
            scoped_lock lock(mutex_);
            if (full_ == BUF_SIZE)
            {
                {
                    boost::mutex::scoped_lock lock(io_mutex);
                    std::cout << "Buffer is full_. Waiting..." << std::endl;
                }

                while (full_ == BUF_SIZE) {
                    cond_.wait(lock);
                }
            }

            buf_[p_] = m;
            p_ = (p_+1) % BUF_SIZE;
            ++full_;
            cond_.notify_one();
        }

        int get()
        {
            scoped_lock lk(mutex_);
            if (full_ == 0)
            {
                {
                    boost::mutex::scoped_lock lock(io_mutex);
                    std::cout << "Buffer is empty. Waiting..." << std::endl;
                }

                while (full_ == 0) {
                    cond_.wait(lk);
                }
            }

            int i = buf_[c_];
            c_ = (c_+1) % BUF_SIZE;
            --full_;
            cond_.notify_one();
            return i;
        }

    private:
        boost::mutex     mutex_;
        boost::condition cond_;
        unsigned int     p_;      // put index
        unsigned int     c_;      // get index
        unsigned int     full_;   // data num
        int buf_[BUF_SIZE];
};

buffer buf;

void writer()
{
    for (int n = 0; n < ITERS; ++n)
    {
        {
            boost::mutex::scoped_lock lock(io_mutex);
            std::cout << "sending: " << n << std::endl;
        }

        buf.put(n);
    }
}

void reader()
{
    for (int x = 0; x < ITERS; ++x)
    {
        int n = buf.get();

        {
            boost::mutex::scoped_lock lock(io_mutex);
            std::cout << "received: " << n << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    boost::thread thrd1(&reader);
    boost::thread thrd2(&writer);

    thrd1.join();
    thrd2.join();

    return 0;
}

