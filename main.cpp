#include <cassert>
#include <chrono>
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

   // Command line argument parsing //
   auto wait_hour_arg = flag::doubleFlag("hours", 1.0,
                                         "Average number of hours you want your sessions to be (Floating point)");
   flag::parse(argc, argv);

   const double WAIT_HOURS = *wait_hour_arg;
   bool file_exists = std::filesystem::exists(FILE_NAME);

   auto command = parseCommand(argv[1]);
   if (command == _number_of_commands_) {
      const std::string error_message = std::format("Could not parse command: {}", argv[1]);
      PRINT(error_message.c_str(), RED);
      flag::outputHelpMessageAndExit();
   }

   if (command == START) {
      if (file_exists) {
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
      if (previous_sessions.back().end_time != -1) {
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

      if (previous_sessions.back().end_time == -1) {
         printf("You still have a session in progress\n");
         return 0;
      }

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);
      commander.start_new_session();
      PRINT("Session resumed 👍", GREEN);
   }

}
