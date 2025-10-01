#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <utility>
#include <filesystem>
#include <random>

#include "FileReadingUtil.h"
#include "SkinningCommander.h"
#include "SkinningSession.h"

using namespace std;

int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("Incorrect number of commands entered. Put either 'start' or 'check'.\n");
      return 1;
   }

   if (std::string(argv[1]) == "start") {
      std::vector<SkinningSession> previous_sessions{};

      SkinningCommander commander = SkinningCommander(1.0 / 60.0, previous_sessions);

      commander.start_new_session();
   } else if (std::string(argv[1]) == "check") {
      std::vector<SkinningSession> previous_sessions = read_sessions_file(FILE_NAME);

      SkinningCommander commander = SkinningCommander(1.0 / 60.0, previous_sessions);

      commander.calculate_available_breaks();
   }
}
