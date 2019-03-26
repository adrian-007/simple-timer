#pragma once

#include <condition_variable>

class Event
{
public:
    Event() = default;

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]()
        {
            return m_event;
        });
    }

    bool wait(const std::chrono::nanoseconds& timeout)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_cv.wait_for(lock, timeout, [&]()
        {
            return m_event;
        });
    }

    void notify()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_event = true;
        m_cv.notify_all();
    }

    void reset()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_event = false;
    }

private:
    bool                       m_event = false;
    std::mutex                 m_mutex;
    std::condition_variable    m_cv;
};
