#include "SkinningController.h"

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

void SkinningController::start_new_interval() {
   auto seconds_since_epoch = seconds(get_seconds_since_epoch());

   _session.get_inteval_list().push_back(
      SkinningInterval(seconds_since_epoch.count(), -1, 0)
   );

   write_session_to_file(_session, _file_name);
}

void SkinningController::end_interval() {
   _session.get_inteval_list().back().end_time = get_seconds_since_epoch();
   write_session_to_file(_session, _file_name);
}

int SkinningController::calculate_available_breaks() {
   SkinningInterval current_interval = _session.get_inteval_list().back();

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

void SkinningController::handle_starting_break() {
   PRINTLN("You may have a break", GREEN);
   PRINTLN("Should you choose to accept one", GREEN);
   getchar();

   _session.get_inteval_list().back().breaks_taken++;
   end_interval();

   atomic_bool break_finished{false};
   long start_time{get_seconds_since_epoch()};

   thread new_thread =
         thread([&start_time, &break_finished] {
            printf("Break started\n");
            while (!break_finished.load()) {
               std::this_thread::sleep_for(1s);

               int seconds_remaining = BREAK_TIME_IN_SECONDS - static_cast<int>(get_seconds_since_epoch() - start_time);
               int mins = seconds_remaining / 60;

               if (seconds_remaining > 0) {
                  seconds_remaining %= 60;
                  printf("Time remaining: %.2d:%.2d \r", mins, seconds_remaining);
               } else {
                  seconds_remaining = abs(seconds_remaining) % 60;
                  mins = abs(mins);
                  printf("Overtime      : %.2d:%.2d \r", mins, seconds_remaining);
               }

               std::flush(std::cout);
            }
         });


   std::cin.get();
   break_finished.exchange(true);
   new_thread.join();
}

bool SkinningController::calculate_session_statistics() {
   if (_session.session_state() == EMPTY) {
      PRINTLN("No session currently running.", RED);
      return false;
   }

   int work_time = 0, break_time = 0;
   long break_start_time = _session.get_inteval_list()[0].start_time;

   for (auto &session: _session.get_inteval_list()) {
      if (session.end_time == -1) {
         session.end_time = get_seconds_since_epoch();
      }

      break_time += static_cast<int>(session.start_time - break_start_time);
      work_time += static_cast<int>(session.end_time - session.start_time);

      break_start_time = session.end_time;
   }

   printf("Time worked   : %d:%.2d:%.2d \n", work_time / 3600, (work_time % 3600) / 60, work_time % 60);
   printf("Time in breaks: %d:%.2d:%.2d \n", break_time / 3600, (break_time % 3600) / 60, break_time % 60);

   return true;
}

bool SkinningController::archive_time_log_file() {
   auto start_time = time_point<system_clock>(seconds(_session.get_inteval_list()[0].start_time));
   string date_string = std::format("{:%Y-%m-%d}", start_time);
   const string archival_location = std::format("{}{}.txt", ARCHIVE_LOCATION, date_string);

   if (std::rename(_file_name.c_str(), archival_location.c_str()) < 0) {
      printf("Error while moving %s to %s\n", _file_name.c_str(), archival_location.c_str());
      printf("Error: %s", strerror(errno));
      return false;
   }

   return true;
}
