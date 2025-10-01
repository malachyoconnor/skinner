#include "SkinningCommander.h"

#include <cassert>
#include <chrono>
#include <fstream>

#include "FileReadingUtil.h"

using namespace std::chrono;
using std::string;

void SkinningCommander::start_new_session() {
   auto seconds_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch());

   assert(_previous_sessions.empty());

   _previous_sessions.push_back(
      SkinningSession{seconds_since_epoch.count(), 0}
   );

   write_sessions_to_file(_previous_sessions, FILE_NAME);
}

void SkinningCommander::end_session() {
}

void SkinningCommander::calculate_available_breaks() {
   SkinningSession current_session = _previous_sessions.back();

   system_clock::time_point start_time(seconds(current_session.start_time));
   auto seconds_since_start = duration_cast<seconds>(system_clock::now() - start_time);

   auto seconds_since_last_break = seconds_since_start;
   std::mt19937 gen(current_session.start_time);

   printf("We read this as your breaks completed %d\n", current_session.breaks_taken);

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
      printf("\033[1;32mYou may have a break\033[0m\n");
      printf("\033[1;32mShould you choose to accept one\033[0m\n");
      getchar();

      _previous_sessions.back().breaks_taken++;
      write_sessions_to_file(_previous_sessions, FILE_NAME);
   } else {
      printf("\033[1;31mGet back to work mutant\033[0m\n");
   }
}
