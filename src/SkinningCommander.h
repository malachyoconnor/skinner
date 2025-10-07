#ifndef SKINNER_SKINNINGCOMMANDER_H
#define SKINNER_SKINNINGCOMMANDER_H
#include <random>

#include "SkinningInterval.h"
#include "SkinningSession.h"

using std::uniform_real_distribution;

constexpr std::string FILE_NAME = "time_log.txt";

class SkinningCommander {
public:
   explicit
   SkinningCommander(double average_hours,
                     SkinningSession* previous_session) : _average_hours(average_hours),
                                                          _session(*previous_session) {
      _distribution = uniform_real_distribution(average_hours * 0.8, average_hours * 1.2);
   }

   void start_new_interval();

   void end_session();

   int calculate_available_breaks();

   void handle_starting_break();

   bool calculate_session_statistics();

   bool archive_time_log_file();

   SkinningCommander() = delete;

private:
   double _average_hours;
   uniform_real_distribution<> _distribution;
   SkinningSession &_session;
};


#endif //SKINNER_SKINNINGCOMMANDER_H
