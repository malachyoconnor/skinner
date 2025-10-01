#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <utility>
#include <filesystem>
#include <random>

#include "src/FileReadingUtil.h"
#include "src/SkinningCommander.h"
#include "src/SkinningSession.h"

using namespace std;

int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("Incorrect number of commands entered. Put either 'start' or 'check'.\n");
      return 1;
   }

   const double WAIT_HOURS = 1.0 / 60.0 / 60.0;

   if (std::string(argv[1]) == "start") {
      std::vector<SkinningSession> previous_sessions{};

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);

      commander.start_new_session();

   } else if (std::string(argv[1]) == "check") {
      std::vector<SkinningSession> previous_sessions = read_sessions_file(FILE_NAME);

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);

      commander.calculate_available_breaks();

   } else if (std::string(argv[1]) == "resume") {
      std::vector<SkinningSession> previous_sessions = read_sessions_file(FILE_NAME);

      if (previous_sessions.back().end_time == -1) {
         printf("You still have a session in progress\n");
         return 0;
      }

      SkinningCommander commander = SkinningCommander(WAIT_HOURS, previous_sessions);

      commander.start_new_session();
   }
}
