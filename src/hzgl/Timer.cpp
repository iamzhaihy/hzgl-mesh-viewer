#include "Timer.hpp"

#include <chrono>

hzgl::SimpleTimer::SimpleTimer()
{
    _timeCreated = std::chrono::high_resolution_clock::now();
    _timePrev = _timeCreated;
}

void hzgl::SimpleTimer::Start()
{
    _timeStart = std::chrono::high_resolution_clock::now();
}

double hzgl::SimpleTimer::End()
{
    // calculate elapsed time (in seconds)
    std::chrono::time_point timeEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = timeEnd - _timeStart;

    // reset the start time to default (0)
    _timeStart = std::chrono::time_point<std::chrono::high_resolution_clock>();

    return elapsedSeconds.count();
}

double hzgl::SimpleTimer::Tick()
{
    std::chrono::time_point timeNow = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> deltaTime = timeNow - _timePrev;

    _timePrev = timeNow;

    return deltaTime.count();
}