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

template <typename Rep, typename Period>
class MinimumPeriodWait
{
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::duration<Rep, Period>;

  public:
    MinimumPeriodWait(const duration& period_duration) : beginning_{clock::now()}, period_duration_{period_duration} {}

    void end_interval()
    {
        std::this_thread::sleep_until(beginning_ + period_duration_);
        const auto now = clock::now();
        elapsed_ = std::chrono::duration_cast<duration>(now - beginning_);
        beginning_ = now;
    }

    [[nodiscard]] duration previous_interval_duration() const noexcept
    {
        return elapsed_;
    }

  private:
    time_point beginning_;
    duration period_duration_;
    duration elapsed_;
};
