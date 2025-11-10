#include <gtest/gtest.h>
#include <stdio.h>
#include <istream>
#include <iostream>

#include "FileReadingUtil.h"
#include "SkinningController.h"
#include "SkinningSession.h"
#include "utils.h"
#include "test_util.h"

// TODO: Tests of these:
// int calculate_available_breaks();
// void handle_starting_break();
// bool calculate_session_statistics();
// bool archive_time_log_file();

TEST(start_new_interval, Success) {
   auto current_time = get_seconds_since_epoch();

   std::vector<SkinningInterval> intervals = {
      get_interval(current_time, current_time + ONE_HOUR),
      get_interval(current_time + ONE_HOUR, current_time + 2 * ONE_HOUR)
   };

   SkinningSession new_session = SkinningSession(intervals);
   SkinningController controller = SkinningController(0, &new_session, TEST_FILE_NAME);
   write_session_to_file(new_session, TEST_FILE_NAME);

   controller.start_new_interval();
   auto [session, error] = read_session_from_file(TEST_FILE_NAME);

   ASSERT_EQ(error, nullptr);
   EXPECT_EQ(session->get_num_intervals(), 3);

   auto last_interval = session->get_interval_list().back();
   EXPECT_EQ(last_interval.end_time, -1);
   EXPECT_GT(last_interval.start_time, -1);
}

TEST(start_new_interval, failure) {
   auto current_time = get_seconds_since_epoch();

   std::vector<SkinningInterval> intervals = {
      get_interval(current_time, current_time + ONE_HOUR),
      get_interval(current_time + ONE_HOUR, -1)
   };

   SkinningSession new_session = SkinningSession(intervals);
   SkinningController controller = SkinningController(0, &new_session, TEST_FILE_NAME);
   write_session_to_file(new_session, TEST_FILE_NAME);

   EXPECT_DEATH({controller.start_new_interval();},
                "Trying to start a new interval when the previous hasn't finished!");
}


TEST(end_interval, Success) {
   auto current_time = get_seconds_since_epoch();

   std::vector<SkinningInterval> intervals = {
      get_interval(current_time, current_time + ONE_HOUR),
      get_interval(current_time + ONE_HOUR, -1)
   };

   SkinningSession new_session = SkinningSession(intervals);
   SkinningController controller = SkinningController(0, &new_session, TEST_FILE_NAME);
   write_session_to_file(new_session, TEST_FILE_NAME);

   controller.end_interval();
   auto [session, error] = read_session_from_file(TEST_FILE_NAME);

   ASSERT_EQ(error, nullptr);
   EXPECT_EQ(session->get_num_intervals(), 2);

   auto last_interval = session->get_interval_list().back();
   EXPECT_NE(last_interval.end_time, -1);
}

TEST(end_interval, failure) {
   auto current_time = get_seconds_since_epoch();

   std::vector<SkinningInterval> intervals = {
      get_interval(current_time, current_time + ONE_HOUR),
      get_interval(current_time + ONE_HOUR, current_time + ONE_HOUR * 2)
   };

   SkinningSession new_session = SkinningSession(intervals);
   SkinningController controller = SkinningController(0, &new_session, TEST_FILE_NAME);
   write_session_to_file(new_session, TEST_FILE_NAME);

   EXPECT_DEATH({controller.end_interval();},
                "Attempting to end an already finished interval");
}
