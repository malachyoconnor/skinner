#ifndef SKINNER_TEST_UTIL_H
#define SKINNER_TEST_UTIL_H

inline const char* TEST_FILE_NAME = "test_time_log.txt";
inline constexpr long ONE_HOUR = 3600;

inline SkinningInterval get_interval(long start, long end) {
   return {
      start, end, 0
   };
}


#endif //SKINNER_TEST_UTIL_H
