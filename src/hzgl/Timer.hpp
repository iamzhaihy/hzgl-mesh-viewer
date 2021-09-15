#pragma once

#include <chrono>

namespace hzgl {
    class SimpleTimer
    {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock>
            _timeCreated,
            _timeStart,        // for recording the duration
            _timePrev;         // for computing delta time

    public:
        SimpleTimer();

        void Start();
        double End();
        double Now();
        double Tick();
    };
} // namespace hzgl