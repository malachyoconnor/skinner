#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <random>

#include "flag.h"
#include "utils.h"
#include "src/FileReadingUtil.h"
#include "src/SkinningCommander.h"
#include "src/SkinningSession.h"

using namespace std;

int main(int argc, const char *argv[]) {
   if (argc == 1) {
      printf("Incorrect number of commands entered. Put either 'start' or 'check'.\n");
      return 1;
   }

   auto wait_hour_arg = flag::doubleFlag("hours", 1.0,
                                         "Average number of hours you want your sessions to be (Floating point)");
   flag::parse(argc, argv);

   const double WAIT_HOURS = *wait_hour_arg;

   auto command = parseCommand(argv[1]);
   if (command == _number_of_commands_) {
      const std::string error_message = std::format("Could not parse command: {}", argv[1]);
      PRINT(error_message.c_str(), RED);
      flag::outputHelpMessageAndExit();
   }

   if (command == START) {
      if (std::filesystem::exists(FILE_NAME)) {
         PRINT("A session already exists, are you sure you want to start a new one?", RED);
         cin.get();
         PRINT("Are you really sure? If not, enter: `skinner resume`", RED);
         cin.get();
      }

      std::vector<SkinningSession> previous_sessions{};

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);

      commander.start_new_session();
   } else if (command == CHECK) {
      std::vector<SkinningSession> previous_sessions = read_sessions_file(FILE_NAME);
      if (previous_sessions.empty() || previous_sessions.back().end_time != -1) {
         PRINT("No session currently running. Start a new session or resume your current one first.", RED);
         exit(EXIT_FAILURE);
      }

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);

      int breaks_allowed = commander.calculate_available_breaks();

      if (breaks_allowed > 0) {
         commander.handle_starting_break();
      } else {
         PRINT("Get back to work mutant", RED);
      }
   } else if (command == RESUME) {
      std::vector<SkinningSession> previous_sessions = read_sessions_file(FILE_NAME);

      if (previous_sessions.empty()) {
         PRINT("No session currently running.", RED);
         exit(EXIT_FAILURE);
      }

      if (previous_sessions.back().end_time == -1) {
         printf("You still have a session in progress\n");
         return 0;
      }

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);
      commander.start_new_session();
      PRINT("Session resumed 👍", GREEN);
   } else if (command == STATS) {
      std::vector<SkinningSession> previous_sessions = read_sessions_file(FILE_NAME);
      if (previous_sessions.empty()) {
         PRINT("No session currently running.", RED);
         exit(EXIT_FAILURE);
      }

      int work_time = 0;
      int break_time = 0;

      long break_start_time = previous_sessions[0].start_time;

      for (auto &session: previous_sessions) {
         if (session.end_time == -1) {
            session.end_time = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
         }

         break_time += static_cast<int>(session.start_time - break_start_time);
         work_time += static_cast<int>(session.end_time - session.start_time);

         printf("start&end :%ld %ld\n", session.start_time, session.end_time);
         printf("break&work: %d %d\n", break_time, work_time);

         break_start_time = session.end_time;
      }

      printf("Time worked   : %d:%.2d:%.2d \n", work_time / 3600, (work_time % 3600) / 60, work_time % 60);
      printf("Time in breaks: %d:%.2d:%.2d \n", break_time / 3600, (break_time % 3600) / 60, break_time % 60);
   } else if (command == FINISH) {
      if (std::filesystem::exists(FILE_NAME)) {
         PRINT("Are you sure you want to finish this session?", RED);
         cin.get();
      }

      std::vector<SkinningSession> previous_sessions = read_sessions_file(FILE_NAME);
      if (previous_sessions.empty() || previous_sessions.back().end_time != -1) {
         PRINT("No session currently running. Start a new session or resume your current one first.", RED);
         exit(EXIT_FAILURE);
      }

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);
      commander.end_session();

      auto now = system_clock::now();
      string dmy = std::format("{:%Y-%m-%d}", now);
      const string archival_location = std::format("{}{}.txt", ARCHIVE_LOCATION, dmy);

      if (std::rename(FILE_NAME.c_str(), archival_location.c_str()) < 0) {
         printf("Error while moving %s to %s\n", FILE_NAME.c_str(), archival_location.c_str());
         printf("Error: %s", strerror(errno));
         exit(EXIT_FAILURE);
      }
   } else {
      string error_msg = std::format("We haven't handled this command: {}", argv[1]);
      throw std::logic_error(error_msg);
   }
}
