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

        Vector3 predict_landing_point();
        double predict_landing_deltatime();

        // Returns an immutable reference to past_pos
        const std::vector<std::pair<double, Vector3>> get_all_past_tvpair();

        // Gives you all past positions as a json
        const std::string get_all_past_tvpair_json();
        const std::string get_tvpair_json(int index);

        // Get the position of the ball closest to abstime
        // abstime refers to seconds elapsed since simulation start
        Vector3 get_tpos(double abstime);

        // Predict the position of the ball deltat seconds from now
        // Negative deltat will return predicted times in the past.
        // Negative deltat is not yet implemented.
        Vector3 predict_pos(double deltat);
        
        // Recorded positions from now on should be contributing
        // to the predicted location of the object
        bool start_prediction(double max_velocity = 0.5);

        // Recorded positions from now on should not be contributing
        // to the predicted location of the object
        bool stop_prediction(int consider_last_n_points = 10, double max_velocity = 0.5);

        // Returns the average velocity of points in between 2 indexes
        // of past_pos array. If no end index is specified, this function
        // defaults to returning the average velocity between the start
        // index and the end of the past_pos array
        Vector3 get_avg_velocity(int start, int end = -1);

        // Samples refers to the number of position vectors considered
        // in the velocity calculation. Passing that variable is not
        // yet implemented.
        // 
        // If deltat = 0, the velocity returned is the most recent.
        // Non-zero deltat is not yet implemented.
        Vector3 get_velocity(double deltat, int samples = 2);
};

#endif
