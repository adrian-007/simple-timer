#include <cassert>

#include "simple_timer.hpp"
#include "event.hpp"

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

    Event evt;

    if (one_shot)
    {
        LOG() << "SimpleTimer::start() - starting one shot timer";
        m_thread = std::thread(std::bind(&SimpleTimer::thread_main_one_shot, this, std::ref(evt), duration, callback));
    }
    else
    {
        LOG() << "SimpleTimer::start() - starting periodic timer";
        m_thread = std::thread(std::bind(&SimpleTimer::thread_main_periodic, this, std::ref(evt), duration, callback));
    }

    LOG() << "SimpleTimer::start() - waiting for timer thread to start...";
    evt.wait();
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

void SimpleTimer::thread_main_periodic(Event& evtStarted, const std::chrono::nanoseconds& duration, const TimerCallback& callback)
{
    assert(callback != nullptr);
    assert(duration != std::chrono::nanoseconds::zero());

    evtStarted.notify();
    while(run_timer(duration, callback));

    LOG() << "SimpleTimer::thread_main_periodic() - timer thread shutting down";
}

void SimpleTimer::thread_main_one_shot(Event& evtStarted, const std::chrono::nanoseconds& duration, const TimerCallback& callback)
{
    assert(callback != nullptr);
    assert(duration != std::chrono::nanoseconds::zero());

    evtStarted.notify();
    run_timer(duration, callback);
    LOG() << "SimpleTimer::thread_main_one_shot() - timer thread shutting down";
}
