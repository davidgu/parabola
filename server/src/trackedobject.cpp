#include "trackedobject.hpp"

// Default constructor doesn't do anything
TrackedObject::TrackedObject(){
    std::vector<std::pair<double, Vector3>> past_pos;
}


void TrackedObject::add_pos(double time, Vector3 cur_pos){
    std::pair<double, Vector3> pair (time, cur_pos);
    past_pos.push_back(pair);
}

Vector3 TrackedObject::predict_landing_point(){
    /// Assume that x and z velocities will remain constant
    // Zero out y velocity, assume that y position is zero at landing point
    Vector3 vel = get_velocity(0);
    vel.y = 0;

    double landing_deltat = predict_landing_deltatime();

    Vector3 pred_pos = past_pos[past_pos.size() - 1].second + (vel*landing_deltat);
    pred_pos.y = 0;

    return pred_pos;
}

double TrackedObject::predict_landing_deltatime(){
    double cur_y = past_pos[past_pos.size() - 1].second.y;
    double deltat = cur_y/grav;
    return deltat;
}

const std::vector<std::pair<double, Vector3>> TrackedObject::get_all_past_tvpair(){
    return past_pos;
}

const std::string TrackedObject::get_all_past_tvpair_json(){
    std::string ret = "{\"data\":[";
    for(int i = past_pos.size() - 1; i>=0; i--){
        ret += get_tvpair_json(i);
    }
    ret += "]}";
    return ret;
}

const std::string TrackedObject::get_tvpair_json(int index){
    std::string str = "{\"time\":"+std::to_string(past_pos[index].first)+",\"pos\":";
    str += past_pos[index].second.to_json();
    str += "}";
    return str;
}

// Template to compare pairs
bool time_pos_compare(const std::pair<double, Vector3> &a,const std::pair<double, Vector3> &b){
    return a.first<b.first;
}

Vector3 TrackedObject::get_tpos(double abstime){
    // Uses lower_bound to find the corresponding Vector3 closest to abstime
    // in the list of past positions
    Vector3 ppos = (std::lower_bound(past_pos.begin(),
                                past_pos.end(),
                                std::make_pair(abstime, Vector3()),
                                time_pos_compare))->second;
    return ppos;
}


bool start_prediction(double min_velocity){
  if(get_velocity(0) > min_velocity) return 1;
}

// This version of start_prediction uses a range instead. We should test both
/*bool start_prediction(int consider_last_n_points, double min_velocity){

  int vec_size = past_pos.size();
  if(!(past_pos.size()>=2)){
    int num_consider = min(consider_last_n_points, vec_size);

    for(int i = vec_size - num_consider; i < vec_size - 1; i++){
      if(get_avg_velocity(i,i+1) < min_velocity) return false;
    }
    return true;
  }else{
    throw std::runtime_error("Cannot calculate velocity when past position vector is empty!");
  }
}*/

bool stop_prediction(int consider_last_n_points, double max_velocity){
  int vec_size = past_pos.size();
  if(!(past_pos.size()>=2)){
    int num_consider = min(consider_last_n_points, vec_size);

    for(int i = vec_size - num_consider; i < vec_size - 1; i++){
      if(get_avg_velocity(i,i+1) > max_velocity) return false;
    }
    return true;
  }else{
    throw std::runtime_error("Cannot calculate velocity when past position vector is empty!");
  }
} 

Vector3 get_avg_velocity(int start, int end){
  int vec_size = past_pos.size();
  if(end == -1) end = vec_size();
  if(end < start) throw std::logic_error("Cannot get average velocity of a negative range");
  if(end > vec_size) throw std::logic_error("Cannot return an average velocity over " + std::to_string(start) + " objects when only " + std::to_string(vec_size) + " objects have been recorded";
  if(start < 0) throw std::logic_error("Cannot get the average of an element with a negative index!");

  double tot_time = 0.0;
  Vector3 first_dist = past_pos[start];
  Vector3 last_dist = past_pos[end - 1];

  for(int i = start; i < end; i++){
    tot_time += past_pos[i].first;
  }
  return (last_dist - first_dist)/tot_time;
}

Vector3 TrackedObject::get_velocity(double deltat, int samples){
    if(!(past_pos.size()>=2)){
        int vec_size = past_pos.size();
        std::pair<double, Vector3> ppos1;
        std::pair<double, Vector3> ppos2;
        if(deltat == 0){
            ppos1 = past_pos[vec_size-1];
            ppos2 = past_pos[vec_size-2];
        }
        else{
            // get_tpos closest to abstime-deltat as ppos1 
            // then get the one before that as ppos2
            throw std::logic_error("Non-zero deltat not yet implemented!");
        }
        Vector3 deltav = ppos1.second - ppos2.second;
        double deltat = ppos1.first - ppos2.first;
        return deltav/deltat;
    }
    else{
        throw std::runtime_error("Cannot calculate velocity when past position vector is empty!");
    }
}

Vector3 TrackedObject::predict_pos(double deltat){
    // s = (1/2)at^2 + vt
    if(deltat >=0){
        Vector3 accel = gravv;
        Vector3 cur_vel = get_velocity(0);
        Vector3 cur_pos = past_pos[past_pos.size() - 1].second;
        return (cur_pos + (accel*0.5)*(deltat*deltat) + (cur_vel*deltat));
    }
    else{
        throw std::logic_error("Negative deltat not yet implemented");
    }
}
