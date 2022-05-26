#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std::chrono_literals;

class ThreadPool
{
public:
    ThreadPool()
    {
        for (int n = 0; n < m_count; n++)
        {
            m_threads.emplace_back(std::thread(&ThreadPool::ThreadFunc, this, n));
        }
    }
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_done = true;
            m_cv.notify_all();
        }
        for (auto &t:m_threads)
            t.join();

    }
    void ThreadFunc(int x)
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            auto done_waiting = m_cv.wait_for(lock, 100ms, [this] { return m_done; });
            if (!done_waiting) // timeout
            {
                std::cout << "Thread x=" << x << std::endl;
            }
            else
                break;
        }
    }
private:
    const int m_count = 5;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_done = false;
};

int main()
{
    std::cout << "Hello World!\n";
    ThreadPool tp;
    std::this_thread::sleep_for(5s);

}
