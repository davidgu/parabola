#ifndef TRACKEDOBJECT_H
#define TRACKEDOBJECT_H

#include <algorithm>
#include <vector>
#include <string>

#include "vector3.hpp"

class TrackedObject{
    private:
        const double grav = 9.81;
        const Vector3 gravv = Vector3(0.0, -grav, 0.0);
        // Stores <time, position>
        std::vector<std::pair<double, Vector3>> past_pos;
        
    public:
        TrackedObject();
        // Adds a position to the list of past positions
        void add_pos(double time, Vector3 cur_pos);

        // Returns an immutable reference to past_pos
        const std::vector<std::pair<double, Vector3>> get_all_past_pos();

        // Gives you all past positions as a json
        const std::string get_all_past_pos_json();

        // Get the position of the ball closest to abstime
        // abstime refers to seconds elapsed since simulation start
        Vector3 get_tpos(double abstime);

        // Predict the position of the ball deltat seconds from now
        // Negative deltat will return predicted times in the past.
        // Negative deltat is not yet implemented.
        Vector3 predict_pos(double deltat);

        // Samples refers to the number of position vectors considered
        // in the velocity calculation. Passing that variable is not
        // yet implemented.
        // 
        // If deltat = 0, the velocity returned is the most recent.
        // Non-zero deltat is not yet implemented.
        Vector3 get_velocity(double deltat, int samples = 2);
};

#endif