#include "SkinningCommander.h"

#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>

#include "FileReadingUtil.h"
#include "utils.h"

using namespace std::chrono;
using std::string, std::thread, std::printf, std::atomic_bool, std::atomic_int;

void SkinningCommander::start_new_interval() {
   auto seconds_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch());

   _session.session_log().push_back(
      SkinningInterval(seconds_since_epoch.count(), -1, 0)
   );

   write_session_to_file(_session, FILE_NAME);
}

void SkinningCommander::end_session() {
   _session.session_log().back().end_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
   write_session_to_file(_session, FILE_NAME);
}

int SkinningCommander::calculate_available_breaks() {
   SkinningInterval current_interval = _session.session_log().back();

   system_clock::time_point start_time(seconds(current_interval.start_time));
   auto seconds_since_start = duration_cast<seconds>(system_clock::now() - start_time);

   auto seconds_since_last_break = seconds_since_start;
   std::mt19937 gen(current_interval.start_time);

   int breaks_allowed = -current_interval.breaks_taken;

   while (seconds_since_last_break.count() > 0) {
      double generated_hours_to_work = _distribution(gen);
      duration<long> work_seconds(static_cast<long>(generated_hours_to_work * 3600));

      seconds_since_last_break -= work_seconds;
      if (seconds_since_last_break.count() > 0) {
         breaks_allowed++;
      }
   }

   return breaks_allowed;
}

void SkinningCommander::handle_starting_break() {
   PRINT("You may have a break", GREEN);
   PRINT("Should you choose to accept one", GREEN);
   getchar();

   atomic_bool break_finished{false};
   atomic_int break_seconds_remaining{10 * 60};

   thread new_thread =
         thread([&break_seconds_remaining, &break_finished] {
            printf("Break started\n");
            while (!break_finished.load()) {
               std::this_thread::sleep_for(1s);
               --break_seconds_remaining;

               int seconds = break_seconds_remaining.load();
               int mins = seconds / 60;

               if (seconds > 0) {
                  seconds %= 60;
                  printf("Time remaining: %.2d:%.2d \r", mins, seconds);
               } else {
                  seconds = abs(seconds) % 60;
                  mins = abs(mins);
                  printf("Overtime      : %.2d:%.2d \r", mins, seconds);
               }

               std::flush(std::cout);
            }

            while (!break_finished.load()) {
               std::this_thread::sleep_for(1s);
               --break_seconds_remaining;

               printf("Seconds remaining: %d\r", break_seconds_remaining.load());
               std::flush(std::cout);
            }
         });


   std::cin.get();
   break_finished.exchange(true);
   new_thread.join();

   _session.session_log().back().breaks_taken++;
   end_session();
}

bool SkinningCommander::calculate_session_statistics() {
   if (_session.session_state() == EMPTY) {
      PRINT("No session currently running.", RED);
      return false;
   }

   int work_time = 0, break_time = 0;
   long break_start_time = _session.session_log()[0].start_time;

   for (auto &session: _session.session_log()) {
      if (session.end_time == -1) {
         session.end_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
      }

      break_time += static_cast<int>(session.start_time - break_start_time);
      work_time += static_cast<int>(session.end_time - session.start_time);

      break_start_time = session.end_time;
   }

   printf("Time worked   : %d:%.2d:%.2d \n", work_time / 3600, (work_time % 3600) / 60, work_time % 60);
   printf("Time in breaks: %d:%.2d:%.2d \n", break_time / 3600, (break_time % 3600) / 60, break_time % 60);

   return true;
}

bool SkinningCommander::archive_time_log_file() {
   auto now = system_clock::now();
   string date_string = std::format("{:%Y-%m-%d}", now);
   const string archival_location = std::format("{}{}.txt", ARCHIVE_LOCATION, date_string);

   if (std::rename(FILE_NAME.c_str(), archival_location.c_str()) < 0) {
      printf("Error while moving %s to %s\n", FILE_NAME.c_str(), archival_location.c_str());
      printf("Error: %s", strerror(errno));
      return false;
   }

   return true;
}
