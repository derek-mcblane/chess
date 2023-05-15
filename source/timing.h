#include <chrono>
#include <thread>

template <typename Rep, typename Period>
class Stopwatch
{
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::duration<Rep, Period>;

    static constexpr auto zero_ms = std::chrono::milliseconds{0};

  public:
    Stopwatch() : beginning_{clock::now()} {}

    void restart()
    {
        beginning_ = clock::now();
    }

    duration elapsed() const
    {
        return clock::now() - beginning_;
    }

  private:
    time_point beginning_;
};

template <typename Rep, typename Period>
class Timer
{
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::duration<Rep, Period>;

  public:
    Timer(std::chrono::duration<Rep, Period> duration) : beginning_{clock::now()}, duration_{duration} {}

    void restart()
    {
        beginning_ = clock::now();
    }

    duration elapsed() const
    {
        return clock::now() - beginning_;
    }

    duration remaining() const
    {
        return duration_ - elapsed();
    }

    [[nodiscard]] bool done() const
    {
        return remaining() <= duration{0};
    }

  private:
    time_point beginning_;
    duration duration_;
};

template <typename Duration>
class MinimumPeriodWait
{
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

  public:
    MinimumPeriodWait(const Duration& period_duration) : beginning_{Clock::now()}, period_duration_{period_duration} {}

    void end_interval()
    {
        std::this_thread::sleep_until(beginning_ + period_duration_);
        const auto now = Clock::now();
        elapsed_ = std::chrono::duration_cast<Duration>(now - beginning_);
        beginning_ = now;
    }

    [[nodiscard]] Duration previous_interval_duration() const noexcept
    {
        return elapsed_;
    }

  private:
    TimePoint beginning_;
    Duration period_duration_;
    Duration elapsed_;
};

template <typename Rep, typename Period>
Rep to_milliseconds(const std::chrono::duration<Rep, Period> duration)
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(duration).count();
}

