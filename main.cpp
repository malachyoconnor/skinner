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

int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("Incorrect number of commands entered. Put either 'start' or 'check'.\n");
      return 1;
   }

   auto testFlag = flag::stringFlag("test", "abc", "What?! HELP");
   flag::parse(argc, argv);


   printf("%s", testFlag->c_str());

   return 0;


   const double WAIT_HOURS = 1.0;

   bool file_exists = std::filesystem::exists(FILE_NAME);

   if (std::string(argv[1]) == "start") {
      if (file_exists) {
         PRINT("A session already exists, are you sure you want to start a new one?", RED);
         cin.get();
         PRINT("Are you really sure? If not, enter: `skinner resume`", RED);
         cin.get();
      }

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
