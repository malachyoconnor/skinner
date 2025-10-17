#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <format>
#include <filesystem>
#include <random>
#include <unistd.h>

#include "flag.h"
#include "utils.h"
#include "FileReadingUtil.h"
#include "SkinningCommander.h"
#include "SkinningSession.h"

using std::cin;
using std::printf;

int main(int argc, const char *argv[]) {
   if (argc == 1) {
      printf("Incorrect number of commands entered. Put either 'start' or 'check'.\n");
      return EXIT_FAILURE;
   }

   auto wait_hour_arg = flag::doubleFlag("hours", 1.0,
                                         "Average number of hours you want your sessions to be (Floating point)");
   auto file_name = flag::stringFlag("file", "./time_log.txt",
                                     "Path to time log file");

   flag::parseFlags(argc, argv);

   const double INTERVAL_LENGTH_HOURS = *wait_hour_arg;
   const string FILE_NAME = *file_name;

   SkinnerCommand command = parseCommand(argv[1]);
   if (command == _number_of_commands_) {
      const std::string error_message = std::format("Could not parse command: {}", argv[1]);
      PRINTLN(error_message.c_str(), RED);
      flag::outputHelpMessageAndExit();
   }

   switch (command) {
      case START_NEW_SESSION: {
         if (std::filesystem::exists(FILE_NAME)) {
            // TODO: Consider renaming the old one, rather than cruelly deleting it
            PRINTLN("A session already exists, are you sure you want to start a new one?", RED);
            cin.get();
            PRINTLN("Are you really sure? If not, enter: `skinner resume`", RED);
            cin.get();
         }

         SkinningSession *new_session = SkinningSession::newSkinningSession();
         SkinningCommander commander = SkinningCommander(INTERVAL_LENGTH_HOURS, new_session, FILE_NAME);
         commander.start_new_interval();
         PRINTLN("New session started 👍", GREEN);
         return EXIT_SUCCESS;
      }

      case CHECK_SESSION: {
         auto [current_session, err] = read_skinning_session(FILE_NAME);

         if (err || current_session->session_state() != IN_PROGRESS) {
            PRINTLN("No session currently running. Start a new session or resume your current one first.", RED);
            return EXIT_FAILURE;
         }

         SkinningCommander commander = SkinningCommander(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         int breaks_allowed = commander.calculate_available_breaks();

         if (breaks_allowed > 0) {
            commander.handle_starting_break();
            commander.start_new_interval();
            PRINTLN("Session resumed 👍", GREEN);
         } else {
            PRINTLN("Get back to work mutant", RED);
         }
         return EXIT_SUCCESS;
      }

      case RESUME_SESSION: {
         auto [current_session, read_successfully] = read_skinning_session(FILE_NAME);

         if (current_session->session_state() == EMPTY) {
            PRINTLN("No session currently running.", RED);
            return EXIT_FAILURE;
         }

         if (current_session->session_state() == IN_PROGRESS) {
            PRINTLN("You still have a session in progress", RED);
            return EXIT_FAILURE;
         }

         SkinningCommander commander = SkinningCommander(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         commander.start_new_interval();
         PRINTLN("Session resumed 👍", GREEN);
         return EXIT_SUCCESS;
      }

      case GET_STATISTICS: {
         auto [current_session, reading_error] = read_skinning_session(FILE_NAME);
         if (reading_error) {
            PRINTLN(reading_error->what(), RED);
            return EXIT_FAILURE;
         }
         SkinningCommander commander = SkinningCommander(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         bool success = commander.calculate_session_statistics();
         return success ? EXIT_SUCCESS : EXIT_FAILURE;
      }

      case FINISH_SESSION: {
         if (std::filesystem::exists(FILE_NAME)) {
            PRINTLN("Are you sure you want to finish this session?", RED);
            cin.get();
         }

         auto [current_session, err] = read_skinning_session(FILE_NAME);
         if (err || current_session->session_state() == EMPTY) {
            PRINTLN("No session currently running. Start a new session first.", RED);
            return EXIT_FAILURE;
         }

         SkinningCommander commander = SkinningCommander(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         commander.end_session();

         bool success = commander.archive_time_log_file();
         return success ? EXIT_SUCCESS : EXIT_FAILURE;
      }

      case POLL: {
         while (true) {

            string msg = std::format("{:%H.%M} - Keep Skinning", system_clock::now());
            PRINT(msg.c_str(), GREEN);
            fflush(stdout);
            sleep(1);
            printf("\r");
         }
         return 0;
      }

      case _number_of_commands_:
         string error_msg = std::format("We haven't handled this command: {}", argv[1]);
         throw std::logic_error(error_msg);
   }

   __builtin_unreachable();

}
