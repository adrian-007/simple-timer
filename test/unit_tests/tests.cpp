#include <condition_variable>
#include <mutex>
#include <chrono>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <simple_timer.hpp>

using namespace testing;

namespace
{
    class Event
    {
    public:
        Event() = default;

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
}

TEST(SimpleTimer, CallingStopWithoutStart)
{
    SimpleTimer t;
    t.stop();
}

TEST(SimpleTimer, OneShotTimerCallbackCalled)
{
    SimpleTimer t;
    Event evt;
    MockFunction<void()> callback;

    EXPECT_CALL(callback, Call()).Times(1).WillOnce(Invoke(&evt, &Event::notify));

    t.start(std::chrono::milliseconds(50u), [&callback]() { callback.Call(); }, true);

    EXPECT_TRUE(evt.wait(std::chrono::seconds(5)));
    t.stop();
}

TEST(SimpleTimer, PeriodicCallCallback10Times)
{
    SimpleTimer t;
    Event evt;
    MockFunction<void()> callback;

    {
        InSequence s;
        EXPECT_CALL(callback, Call()).Times(9);
        EXPECT_CALL(callback, Call()).Times(1).WillOnce(Invoke(&evt, &Event::notify));
        EXPECT_CALL(callback, Call()).Times(AnyNumber());
    }

    t.start(std::chrono::milliseconds(50u), [&callback]() { callback.Call(); }, false);

    EXPECT_TRUE(evt.wait(std::chrono::seconds(5)));
    t.stop();
}

TEST(SimpleTimer, StartOneShotThenPeriodicOnSameObject)
{
    SimpleTimer t;
    Event evt;
    MockFunction<void()> callback;

    {
        InSequence s;
        EXPECT_CALL(callback, Call()).Times(1).WillOnce(Invoke(&evt, &Event::notify));
        EXPECT_CALL(callback, Call()).Times(9);
        EXPECT_CALL(callback, Call()).Times(1).WillOnce(Invoke(&evt, &Event::notify));
        EXPECT_CALL(callback, Call()).Times(AnyNumber());
    }

    t.start(std::chrono::milliseconds(50u), [&callback]() { callback.Call(); }, true);

    EXPECT_TRUE(evt.wait(std::chrono::seconds(5)));
    evt.reset();

    t.start(std::chrono::milliseconds(50u), [&callback]() { callback.Call(); }, false);

    EXPECT_TRUE(evt.wait(std::chrono::seconds(5)));

    t.stop();
}

int main(int argc, char **argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
