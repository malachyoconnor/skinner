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
using colours::PRINT, colours::PRINTLN;
using enum colours::Colour;

void SkinningController::start_new_interval() {
   std::vector<SkinningInterval> &interval_list = _session.GetIntervalList();
   assert((interval_list.size() == 0 || _session.GetIntervalList().back().end_time != -1)
      && "Trying to start a new interval when the previous hasn't finished!");

   const auto seconds_since_epoch = seconds(get_seconds_since_epoch());
   interval_list.push_back(
      SkinningInterval(seconds_since_epoch.count(), -1, 0)
   );

   write_session_to_file(_session, _file_name);
}

void SkinningController::end_interval() {
   SkinningInterval &final_interval = _session.GetIntervalList().back();
   assert(final_interval.end_time == -1 && "Attempting to end an already finished interval");

   final_interval.end_time = std::max(get_seconds_since_epoch(), final_interval.start_time);
   write_session_to_file(_session, _file_name);
}

int SkinningController::calculate_available_breaks() {
   const SkinningInterval current_interval = _session.GetIntervalList().back();

   const system_clock::time_point start_time(seconds(current_interval.start_time));
   const auto seconds_since_start = duration_cast<seconds>(system_clock::now() - start_time);

   auto seconds_since_last_break = seconds_since_start;
   std::mt19937 gen(current_interval.start_time);

   int breaks_allowed = -current_interval.breaks_taken;

   while (seconds_since_last_break.count() > 0) {
      const double generated_hours_to_work = _distribution(gen);
      const duration<long> work_seconds(static_cast<long>(generated_hours_to_work * 3600));

      seconds_since_last_break -= work_seconds;
      if (seconds_since_last_break.count() > 0) {
         breaks_allowed++;
      }
   }

   return breaks_allowed;
}

void SkinningController::handle_starting_break() {
   PRINTLN("You may have a break", Green);
   PRINTLN("Should you choose to accept one", Green);
   getchar();

   _session.GetIntervalList().back().breaks_taken++;
   end_interval();

   atomic_bool break_finished{false};
   long start_time{get_seconds_since_epoch()};

   auto new_thread =
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
   if (_session.GetSessionState() == EMPTY) {
      PRINTLN("No session currently running.", Red);
      return false;
   }

   const long work_time = _session.CalculateTotalWorkTime();
   const long break_time = _session.CalculateTotalBreakTime();

   printf("Time worked   : %ld:%.2ld:%.2ld \n", work_time / 3600, work_time % 3600 / 60, work_time % 60);
   printf("Time in breaks: %ld:%.2ld:%.2ld \n", break_time / 3600, break_time % 3600 / 60, break_time % 60);

   return true;
}

bool SkinningController::archive_time_log_file() {
   auto start_time = time_point<system_clock>(seconds(_session.GetIntervalList()[0].start_time));
   string date_string = std::format("{:%Y-%m-%d}", start_time);
   const string archival_location = std::format("{}{}.txt", ARCHIVE_LOCATION, date_string);

   if (std::rename(_file_name.c_str(), archival_location.c_str()) < 0) {
      printf("Error while moving %s to %s\n", _file_name.c_str(), archival_location.c_str());
      printf("Error: %s", strerror(errno));
      return false;
   }

   return true;
}
