#include "SkinningCommander.h"

#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "FileReadingUtil.h"
#include "utils.h"

using namespace std::chrono;
using std::string, std::thread, std::printf, std::atomic_bool, std::atomic_int;

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

int SkinningCommander::calculate_available_breaks() {
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

   return breaks_allowed;
}

void SkinningCommander::handle_starting_break() {
   PRINT("You may have a break", GREEN);
   PRINT("Should you choose to accept one", GREEN);
   getchar();

   atomic_bool break_finished{false};
   atomic_int break_seconds_remaining{ 10};

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

   _previous_sessions.back().breaks_taken++;
   end_session();
}
