#ifndef SKINNER_SKINNINGCOMMANDER_H
#define SKINNER_SKINNINGCOMMANDER_H
#include <random>

#include "SkinningSession.h"

using std::uniform_real_distribution;

constexpr std::string FILE_NAME = "start.txt";

class SkinningCommander {
public:
   explicit
   SkinningCommander(double average_hours,
                     const std::vector<SkinningSession> &previous_sessions) : _average_hours(average_hours),
                                                                              _previous_sessions(previous_sessions) {
      _distribution = uniform_real_distribution(average_hours * 0.8, average_hours * 1.2);
   }

   void start_new_session();

   void end_session();

   void calculate_available_breaks();

   SkinningCommander() = delete;

private:
   double _average_hours;
   uniform_real_distribution<> _distribution;
   std::vector<SkinningSession> _previous_sessions;
};


#endif //SKINNER_SKINNINGCOMMANDER_H
