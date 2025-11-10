#include <gtest/gtest.h>
#include <stdio.h>
#include <istream>
#include <iostream>

#include "FileReadingUtil.h"
#include "SkinningController.h"
#include "SkinningSession.h"
#include "utils.h"
#include "test_util.h"

std::string exec(std::string &cmd) {
   FILE *pipe = popen(cmd.c_str(), "r");
   if (!pipe) {
      EXPECT_NE(pipe, nullptr);
      exit(1);
   }

   std::string result{};
   std::array<char, 128> buffer;
   while (fgets(buffer.begin(), 128, pipe) != nullptr) {
      for (char c: buffer) {
         if (c == '\0') break;
         result += c;
      }
   }
   return result;
}

TEST(STAT, StraightForwardStats) {

   auto current_time = get_seconds_since_epoch();

   std::vector<SkinningInterval> intervals = {
      get_interval(current_time, current_time + ONE_HOUR),
      get_interval(current_time + ONE_HOUR, current_time + 2*ONE_HOUR)
   };

   SkinningSession new_session = SkinningSession(intervals);
   SkinningController controller = SkinningController(0, &new_session, TEST_FILE_NAME);
   write_session_to_file(new_session, TEST_FILE_NAME);

   std::string cmd1 = std::format("{} stat -file=time_log.txt", SKINNER_EXECUTABLE);
   std::cout << exec(cmd1);
}
