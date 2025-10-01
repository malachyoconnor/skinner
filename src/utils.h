#ifndef SKINNER_UTILS_H
#define SKINNER_UTILS_H
#include <string>

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
   printf("%s%s\n%s", start(col), str, end());
}


#endif //SKINNER_UTILS_H
