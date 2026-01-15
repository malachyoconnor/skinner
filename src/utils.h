#ifndef SKINNER_UTILS_H
#define SKINNER_UTILS_H
#include <algorithm>
#include <string>

#include "SkinningInterval.h"

enum SkinnerCommand {
   START_NEW_SESSION,
   CHECK_SESSION,
   RESUME_SESSION,
   GET_STATISTICS,
   FINISH_SESSION,
   POLL,
   VISUALISE,
   _number_of_commands_
};

inline std::string COMMAND_LIST[_number_of_commands_] = {
   "start",
   "check",
   "resume",
   "stat",
   "finish",
   "poll",
   "visualise"
};

inline SkinnerCommand parseCommand(std::string cmd) {
   std::ranges::transform(cmd, cmd.begin(), tolower);

   for (int i = 0; i < _number_of_commands_; i++) {
      if (cmd == COMMAND_LIST[i]) {
         return static_cast<SkinnerCommand>(i);
      }
   }

   // TODO: This is nasty and there must be another way.
   // The few options I can think of are:
   // 1: Return a std::optional<SkinnerCommand> - this is nasty, too much code for something simple
   // 2: Have an enum that represents a failure to parse... _number_of_commands_ is pushing it
   //    for me. FAILED is not a command
   // 3: Have this function exit the program. This is maybe the nicest, but seems random
   //    that a command parsing function would quite your program
   return _number_of_commands_;
}

inline long get_seconds_since_epoch() {

   return duration_cast<seconds>(
      system_clock::now().time_since_epoch()
   ).count();
}

inline std::string get_human_readable_time(const system_clock::time_point tp) {
   const auto utc_offset = std::chrono::current_zone()->get_info(tp).offset;

   std::string human_readable_time = std::format("{0:%R}", tp + utc_offset);
   return human_readable_time;
}

namespace colours {
   enum Colour {
      Red, Green, Default
   };

#define ESCAPE_CHARACTER "\033["

   inline const char *start(const Colour col) {
      switch (col) {
         case Red: return ESCAPE_CHARACTER "31m";
         case Green: return ESCAPE_CHARACTER "32m";
         case Default: return "";
      }

      __builtin_unreachable();
   }

   inline const char *end() {
      return "\033[0m";
   }

   inline std::string getCurrentTimeString() {
      const string current_time = std::format("{:%d/%m/%y %H:%M}", system_clock::now());
      return std::format("[{}{}{}] ", start(Green), current_time.c_str(), end());
   }

   inline void PRINT(const char *str, const Colour col) {
      std::fputs(getCurrentTimeString().c_str(), stdout);
      std::printf("%s%s%s", start(col), str, end());
   }

   inline void PRINTLN(const char *str, const Colour col) {
      std::fputs(getCurrentTimeString().c_str(), stdout);
      std::printf("%s%s\n%s", start(col), str, end());
   }
}

#endif //SKINNER_UTILS_H
