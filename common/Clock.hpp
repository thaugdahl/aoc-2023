#ifndef CLOCK_HPP_HF7BWZV0
#define CLOCK_HPP_HF7BWZV0


#include <chrono>

template <typename Clock_T = std::chrono::high_resolution_clock, typename DurationType = std::chrono::microseconds >
class Clock
{
public:
    using TimePointType = decltype(Clock_T::now());

    void start();

    const uint64_t elapsed() const noexcept;

    const uint64_t stop();

private:
    TimePointType now() const;

    TimePointType start_time;
    TimePointType end_time;
};

template class Clock<std::chrono::high_resolution_clock, std::chrono::microseconds>;

using MicrosecondClock = Clock<std::chrono::high_resolution_clock, std::chrono::microseconds>;

#endif /* end of include guard: CLOCK_HPP_HF7BWZV0 */
