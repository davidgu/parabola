#ifndef TRACKEDOBJECT_H
#define TRACKEDOBJECT_H

#include <vector>

#include "vector3.hpp"

class TrackedObject{
    private:
        const double grav = 9.81;
        // Stores <time, position>
        std::vector<std::pair<long long, Vector3>> past_pos;
        Vector3 velocity;
        
    public:
        TrackedObject();
        // Adds a position to the list of past positions
        void add_pos(long long time, Vector3 cur_pos);

        // Get the position of the ball closest to abstime
        Vector3 get_tpos(double abstime);
        Vector3 predict_pos(double deltat);
};

#endif