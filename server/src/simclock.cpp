#include "simclock.hpp"

SimClock::SimClock(){
    timeval time;
    gettimeofday(&time, NULL);
    double ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    start = ms;

}

double SimClock::get_abstime(){
    timeval time;
    gettimeofday(&time, NULL);
    double ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (ms-start)/1000;
}