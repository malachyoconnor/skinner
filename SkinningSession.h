#ifndef SKINNER_SKINNINGSESSION_H
#define SKINNER_SKINNINGSESSION_H
#include <chrono>

using std::chrono::seconds;

struct SkinningSession {
   long start_time;
   int breaks_taken;
};

#endif //SKINNER_SKINNINGSESSION_H