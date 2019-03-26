#include <cassert>

#include "simple_timer.hpp"

// Some may consider it as oddity, but it's just for sake of simplicity.
#include "log.cpp"

SimpleTimer::~SimpleTimer()
{
    assert(!m_thread.joinable());
}

void SimpleTimer::start(const std::chrono::nanoseconds& duration, const TimerCallback& callback, bool one_shot /*= false*/)
{
    LOG() << "SimpleTimer::start()";

    assert(callback != nullptr);
    assert(duration != std::chrono::nanoseconds::zero());

    stop();

    std::unique_lock<std::mutex> lock(m_mutex);
    m_stop = false;

    if (one_shot)
    {
        LOG() << "SimpleTimer::start() - starting one shot timer";
        m_thread = std::thread(std::bind(&SimpleTimer::thread_main_one_shot, this, duration, callback));
    }
    else
    {
        LOG() << "SimpleTimer::start() - starting periodic timer";
        m_thread = std::thread(std::bind(&SimpleTimer::thread_main_periodic, this, duration, callback));
    }

    LOG() << "SimpleTimer::start() - waiting for timer thread to start...";
    m_cv.wait(lock);
}

void SimpleTimer::stop()
{
    LOG() << "SimpleTimer::stop()";

    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (!m_thread.joinable())
        {
            LOG() << "SimpleTimer::stop() - timer thread is already joined or hasn't been started yet";
            return;
        }

        LOG() << "SimpleTimer::stop() - setting stop flag to true and notifying timer thread...";
        m_stop = true;
        m_cv.notify_one();
    }

    m_thread.join();
    LOG() << "SimpleTimer::stop() - timer thread joined";
}

bool SimpleTimer::run_timer(const std::chrono::nanoseconds& duration, const TimerCallback& callback)
{
    assert(callback != nullptr);
    assert(duration != std::chrono::nanoseconds::zero());

    bool should_continue;

    {
        std::unique_lock<std::mutex> lock(m_mutex);

        should_continue = !m_cv.wait_for(lock, duration, [&]()
        {
            return m_stop;
        });
    }

    if (should_continue)
    {
        LOG() << "SimpleTimer::run_timer() - calling a callback";
        callback();
    }

    return should_continue;
}

void SimpleTimer::thread_main_periodic(const std::chrono::nanoseconds& duration, const TimerCallback& callback)
{
    assert(callback != nullptr);
    assert(duration != std::chrono::nanoseconds::zero());

    m_cv.notify_one();
    while(run_timer(duration, callback));

    LOG() << "SimpleTimer::thread_main_periodic() - timer thread shutting down";
}

void SimpleTimer::thread_main_one_shot(const std::chrono::nanoseconds& duration, const TimerCallback& callback)
{
    assert(callback != nullptr);
    assert(duration != std::chrono::nanoseconds::zero());

    m_cv.notify_one();
    run_timer(duration, callback);
    LOG() << "SimpleTimer::thread_main_one_shot() - timer thread shutting down";
}
