#ifndef SKINNER_SKINNINGCONTROLLER_H
#define SKINNER_SKINNINGCONTROLLER_H
#include <random>

#include "SkinningInterval.h"
#include "SkinningSession.h"

using std::uniform_real_distribution;

constexpr int BREAK_TIME_IN_SECONDS = 10 * 60;

class SkinningController {
public:
   SkinningController() = delete;

   explicit SkinningController(double average_hours,
                              SkinningSession *previous_session,
                              std::string file_name) : _average_hours(average_hours),
                                                  _session(*previous_session),
                                                  _file_name(std::move(file_name)) {
      _distribution = uniform_real_distribution(average_hours * 0.8, average_hours * 1.2);
   }

   void start_new_interval();

   void end_interval();

   int calculate_available_breaks();

   void handle_starting_break();

   bool calculate_session_statistics();

   bool archive_time_log_file();

private:
   double _average_hours;
   uniform_real_distribution<> _distribution;
   SkinningSession &_session;
   std::string _file_name;
};


#endif //SKINNER_SKINNINGCONTROLLER_H
