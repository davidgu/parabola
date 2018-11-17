#ifndef SIMCLOCK_H
#define SIMCLOCK_H

#include <time.h>

// Encapsulates the C++ clock, returning values in terms
// of seconds instead of clock ticks
class SimClock{
    private:
        double CPS = CLOCKS_PER_SEC;
        clock_t start;

    public:
        // Returns time elapsed since the start of the simulation
        double get_abstime();
};

#endif