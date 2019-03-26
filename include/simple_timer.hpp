#ifndef SIMPLE_TIMER_HPP
#define SIMPLE_TIMER_HPP

#include <functional>
#include <thread>
#include <condition_variable>

class Event;
using TimerCallback = std::function<void()>;

class SimpleTimer
{
public:
    SimpleTimer() = default;
    ~SimpleTimer();

    void start(const std::chrono::nanoseconds& duration, const TimerCallback& callback, bool one_shot = false);
    void stop();

private:
    bool run_timer(const std::chrono::nanoseconds& duration, const TimerCallback& callback);
    void thread_main_periodic(Event& evtStarted, const std::chrono::nanoseconds& duration, const TimerCallback& callback);
    void thread_main_one_shot(Event& evtStarted, const std::chrono::nanoseconds& duration, const TimerCallback& callback);

    std::thread                m_thread;
    bool                       m_stop = true;
    std::mutex                 m_mutex;
    std::condition_variable    m_cv;
};

#endif // SIMPLE_TIMER_HPP
