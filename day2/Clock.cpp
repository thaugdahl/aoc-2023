#include "Clock.hpp"

// template <typename Clock_T = std::chrono::high_resolution_clock, typename DurationType = std::chrono::microseconds >


template <typename Clock_T, typename DurationType >
void Clock<Clock_T, DurationType>::start() {
    start_time = now();
}

template <typename Clock_T, typename DurationType >
const uint64_t Clock<Clock_T, DurationType>::elapsed() const noexcept {
        TimePointType currentTimePoint = now();

        if ( end_time > currentTimePoint )  {
            currentTimePoint = end_time;
        }

        uint64_t duration = std::chrono::duration_cast<DurationType>(currentTimePoint - start_time).count();

        return duration;
}


template <typename Clock_T, typename DurationType >
const uint64_t Clock<Clock_T, DurationType>::stop()  {
        end_time = now();
        uint64_t duration = std::chrono::duration_cast<DurationType>(end_time - start_time).count();
        return duration;
}

template <typename Clock_T, typename DurationType >
decltype(Clock_T::now()) Clock<Clock_T, DurationType>::now() const  {
    return Clock_T::now();
}

