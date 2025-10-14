#ifndef SKINNER_UTILS_H
#define SKINNER_UTILS_H
#include <algorithm>
#include <string>

enum SkinnerCommand {
   START_NEW_SESSION,
   CHECK_SESSION,
   RESUME_SESSION,
   GET_STATISTICS,
   FINISH_SESSION,
   _number_of_commands_
};

inline std::string COMMAND_LIST[_number_of_commands_] = {
   "start",
   "check",
   "resume",
   "stat",
   "finish"
};

inline SkinnerCommand parseCommand(std::string cmd) {
   std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

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

enum Colour {
   RED, GREEN
};

#define START_COLOUR "\033["

inline const char *start(Colour col) {
   switch (col) {
      case RED: return START_COLOUR "31m";
      case GREEN: return START_COLOUR "32m";
   }

   __builtin_unreachable();
}

inline const char *end() {
   return "\033[0m";
}

inline void PRINT(const char *str, Colour col) {
   std::printf("%s%s\n%s", start(col), str, end());
}

inline long get_seconds_since_epoch() {
   return duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()
   ).count();
}

#endif //SKINNER_UTILS_H
