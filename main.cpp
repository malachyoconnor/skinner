#include <chrono>
#include <fstream>
#include <iostream>
#include <format>
#include <filesystem>
#include <random>
#include <unistd.h>
#include <WorkVisualiser.h>

#include "flag.h"
#include "utils.h"
#include "FileReadingUtil.h"
#include "SkinningController.h"
#include "SkinningSession.h"

using std::cin;
using std::printf;
using colours::PRINT, colours::PRINTLN;

int main(int argc, const char *argv[]) {
   if (argc == 1) {
      PRINTLN("Incorrect number of commands entered.", Red);
      flag::outputHelpMessageAndExit();

      return EXIT_FAILURE;
   }

   auto wait_hour_arg =
         flag::doubleFlag("hours", 1.0,
                          "Average number of hours you want your sessions to be (Floating point)");
   auto file_name =
         flag::stringFlag("file", "./time_log.txt", "Path to time log file");

   flag::parseFlags(argc, argv);

   const double INTERVAL_LENGTH_HOURS = *wait_hour_arg;
   const string FILE_NAME = *file_name;

   SkinnerCommand command = parseCommand(argv[1]);
   if (command == _number_of_commands_) {
      const std::string error_message = std::format("Could not parse command: {}", argv[1]);
      PRINTLN(error_message.c_str(), Red);
      flag::outputHelpMessageAndExit();
   }

   switch (command) {
      case START_NEW_SESSION: {
         if (std::filesystem::exists(FILE_NAME)) {
            PRINTLN("A session already exists, are you sure you want to start a new one?", Green);
            cin.get();
            PRINTLN("Are you really sure? If not, enter: `skinner resume`", Red);
            cin.get();

            if (std::filesystem::exists(FILE_NAME)) {
               std::filesystem::rename(FILE_NAME, FILE_NAME + ".old");
            }
            std::string rename_string = std::format("Renamed to: {}", FILE_NAME + ".old");
            PRINTLN(rename_string.c_str(), Green);
         }

         SkinningSession *new_session = SkinningSession::newSkinningSession();
         SkinningController controller = SkinningController(INTERVAL_LENGTH_HOURS, new_session, FILE_NAME);
         controller.start_new_interval();
         PRINTLN("New session started 👍", Green);
         return EXIT_SUCCESS;
      }

      case CHECK_SESSION: {
         auto [current_session, err] = read_session_from_file(FILE_NAME);

         if (err || current_session->GetSessionState() != IN_PROGRESS) {
            PRINTLN("No session currently running. Start a new session or resume your current one first.", Red);
            return EXIT_FAILURE;
         }

         SkinningController controller = SkinningController(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         int breaks_allowed = controller.calculate_available_breaks();

         if (breaks_allowed > 0) {
            controller.handle_starting_break();
            controller.start_new_interval();
            PRINTLN("Session resumed 👍", Green);
         } else {
            PRINTLN("Get back to work mutant", Red);
         }
         return EXIT_SUCCESS;
      }

      case RESUME_SESSION: {
         auto [current_session, read_successfully] = read_session_from_file(FILE_NAME);

         if (current_session->GetSessionState() == EMPTY) {
            PRINTLN("No session currently running.", Red);
            return EXIT_FAILURE;
         }

         if (current_session->GetSessionState() == IN_PROGRESS) {
            PRINTLN("You still have a session in progress", Red);
            return EXIT_FAILURE;
         }

         SkinningController controller = SkinningController(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         controller.start_new_interval();
         PRINTLN("Session resumed 👍", Green);
         return EXIT_SUCCESS;
      }

      case GET_STATISTICS: {
         auto [current_session, reading_error] = read_session_from_file(FILE_NAME);
         if (reading_error) {
            PRINTLN(reading_error->what(), Red);
            return EXIT_FAILURE;
         }
         SkinningController controller = SkinningController(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         bool success = controller.calculate_session_statistics();
         return success ? EXIT_SUCCESS : EXIT_FAILURE;
      }

      case FINISH_SESSION: {
         auto [current_session, err] = read_session_from_file(FILE_NAME);
         if (err || current_session->GetSessionState() == EMPTY) {
            PRINTLN("No session currently running. Start a new session first.", Red);
            return EXIT_FAILURE;
         }

         for (auto interval: current_session->GetIntervalList()) {
            auto start_time = time_point<system_clock>(seconds(interval.start_time));
            auto end_time = time_point<system_clock>(
               seconds(interval.end_time == -1 ? get_seconds_since_epoch() : interval.end_time));

            string time_range = std::format("{:%d/%m/%y %H:%M} - {:%d/%m/%y %H:%M}", start_time, end_time);
            std::printf("%s\n", time_range.c_str());
         }

         SkinningController controller = SkinningController(INTERVAL_LENGTH_HOURS, current_session, FILE_NAME);
         controller.calculate_session_statistics();

         PRINTLN("Are you sure you want to finish this session? Otherwise Ctrl+C", Red);
         cin.get();

         if (current_session->GetIntervalList().back().end_time == -1) {
            controller.end_interval();
         }

         return controller.archive_time_log_file() ? EXIT_SUCCESS : EXIT_FAILURE;
      }

      case POLL: {
         while (true) {

            string msg = std::format("{:%H.%M} - Keep Skinning", system_clock::now());
            PRINT(msg.c_str(), Green);
            fflush(stdout);
            sleep(1);
            printf("\r");
         }
         return 0;
      }

      case VISUALISE: {
         WorkVisualiser vis = WorkVisualiser();

         // TODO: Slider to restrict dates considered
         // TODO: Mode where breaks are intersliced within the bar.
         //       I.e. parts of the bar are grey where we're taking a break.
         vis.Render();

         return 0;
      }

      case _number_of_commands_:
         string error_msg = std::format("We haven't handled this command: {}", argv[1]);
         throw std::logic_error(error_msg);
   }

   __builtin_unreachable();

}
