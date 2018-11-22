#ifndef SIMCLOCK_H
#define SIMCLOCK_H

#include <time.h>
#include <sys/time.h>

// Encapsulates the C++ clock, returning values in terms
// of seconds instead of clock ticks
class SimClock{
    private:
        double start;

    public:
        SimClock();
        // Returns time elapsed since the start of the simulation
        double get_abstime();
};

#endif