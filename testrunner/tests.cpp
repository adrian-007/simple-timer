#include <simple_timer.hpp>
#include <event.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

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
