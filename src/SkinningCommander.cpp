#include "SkinningCommander.h"

#include <cassert>
#include <chrono>
#include <fstream>

#include "FileReadingUtil.h"
#include "utils.h"

using namespace std::chrono;
using std::string;

void SkinningCommander::start_new_session() {
   auto seconds_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch());

   _previous_sessions.push_back(
      SkinningSession(seconds_since_epoch.count(), -1, 0)
   );

   write_sessions_to_file(_previous_sessions, FILE_NAME);
}

void SkinningCommander::end_session() {
   _previous_sessions.back().end_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
   write_sessions_to_file(_previous_sessions, FILE_NAME);
}

void SkinningCommander::calculate_available_breaks() {
   SkinningSession current_session = _previous_sessions.back();

   system_clock::time_point start_time(seconds(current_session.start_time));
   auto seconds_since_start = duration_cast<seconds>(system_clock::now() - start_time);

   auto seconds_since_last_break = seconds_since_start;
   std::mt19937 gen(current_session.start_time);

   int breaks_allowed = -current_session.breaks_taken;

   while (seconds_since_last_break.count() > 0) {
      double generated_hours_to_work = _distribution(gen);
      duration<long> work_seconds(static_cast<long>(generated_hours_to_work * 3600));

      seconds_since_last_break -= work_seconds;
      if (seconds_since_last_break.count() > 0) {
         breaks_allowed++;
      }
   }

   if (breaks_allowed > 0) {
      PRINT("You may have a break", GREEN);
      PRINT("Should you choose to accept one", GREEN);
      getchar();

      _previous_sessions.back().breaks_taken++;
      end_session();
   } else {
      PRINT("Get back to work mutant", RED);
   }
}
