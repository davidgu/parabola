#include "simclock.hpp"

SimClock::SimClock(){
    start = clock();
}

double SimClock::get_abstime(){
    return (start-clock())/CPS;
}