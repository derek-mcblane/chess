#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>

class Stopwatch
{
  public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    Stopwatch(TimePoint start_time = Clock::now()) : start_time_{start_time} {}

    [[nodiscard]] TimePoint start_time() const
    {
        return start_time_;
    }

    void restart(TimePoint start_time = Clock::now())
    {
        start_time_ = start_time;
    }

    [[nodiscard]] Duration elapsed() const
    {
        return std::chrono::duration_cast<Duration>(Clock::now() - start_time_);
    }

  private:
    TimePoint start_time_;
};

class Timer
{
  public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    template <typename Rep, typename Period>
    Timer(std::chrono::duration<Rep, Period> duration, TimePoint start_time = Clock::now())
        : start_time_{start_time}, duration_{std::chrono::duration_cast<Duration>(duration)}
    {}

    void restart(TimePoint start_time = Clock::now())
    {
        start_time_ = start_time;
    }

    [[nodiscard]] Duration elapsed() const
    {
        return Clock::now() - start_time_;
    }

    [[nodiscard]] Duration remaining() const
    {
        return duration_ - elapsed();
    }

    [[nodiscard]] bool done() const
    {
        return remaining() <= Duration{0};
    }

    void wait_until_done() const
    {
        std::this_thread::sleep_until(done_time());
    }

    void wait_until_done_and_restart()
    {
        std::this_thread::sleep_until(done_time());
        restart(done_time());
    }

  private:
    [[nodiscard]] TimePoint done_time() const
    {
        return start_time_ + duration_;
    }

    TimePoint start_time_;
    Duration duration_;
};
