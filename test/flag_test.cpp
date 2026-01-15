#include <gtest/gtest.h>
#include "flag.h"

// Options for the flag.h library:
// --flag
// -flag
// -flag=x
// -flag x NOT IMP

// BOOLEAN_FLAGS //

TEST(BooleanFlags, One_DoubleDashFlag) {
   constexpr int argc = 2;
   const char* argv[argc] = {"/some_directory", "--TESTING"};

   const auto TEST_FLAG = flag::booleanFlag("TESTING", false, "This is a flag");
   flag::parseFlags(argc, argv);
   EXPECT_EQ(*TEST_FLAG, true);
}

TEST(BooleanFlags, Two_DoubleDashFlag) {
   constexpr int argc = 3;
   const char* argv[argc] = {"/some_directory", "--TESTING_ONE", "--TESTING_TWO"};

   const auto TEST_FLAG_1 = flag::booleanFlag("TESTING_ONE", false, "This is a flag");
   const auto TEST_FLAG_2 = flag::booleanFlag("TESTING_TWO", false, "This is a flag");

   flag::parseFlags(argc, argv);
   EXPECT_EQ(*TEST_FLAG_1, true);
   EXPECT_EQ(*TEST_FLAG_2, true);
}

TEST(BooleanFlags, One_SingleDashFlag) {
   constexpr int argc = 2;
   const char* argv[argc] = {"/some_directory", "-TESTING"};

   const auto TEST_FLAG = flag::booleanFlag("TESTING", false, "This is a flag");
   flag::parseFlags(argc, argv);
   EXPECT_EQ(*TEST_FLAG, true);
}

TEST(BooleanFlags, Two_SingleAndDoubleDashFlags) {
   constexpr int argc = 3;
   const char* argv[argc] = {"/some_directory", "-TESTING_ONE", "--TESTING_TWO"};

   const auto TEST_FLAG_1 = flag::booleanFlag("TESTING_ONE", false, "This is a flag");
   const auto TEST_FLAG_2 = flag::booleanFlag("TESTING_TWO", false, "This is a flag");

   flag::parseFlags(argc, argv);
   EXPECT_EQ(*TEST_FLAG_1, true);
   EXPECT_EQ(*TEST_FLAG_2, true);
}


// INTEGER_FLAGS //

TEST(IntegerFlags, OneSingleDashFlag) {
   constexpr int argc = 2;
   const char* argv[argc] = {"/some_directory", "-TESTING=5"};

   const auto TEST_FLAG = flag::intFlag("TESTING", 0, "This is a flag");
   flag::parseFlags(argc, argv);
   EXPECT_EQ(*TEST_FLAG, 5);
}

// DOUBLE_FLAGS

TEST(DoubleFlags, OneSingleDashFlag) {
   constexpr int argc = 2;
   const char* argv[argc] = {"/some_directory", "-TESTING=5.12"};

   const auto TEST_FLAG = flag::doubleFlag("TESTING", 0.123, "This is a flag");
   flag::parseFlags(argc, argv);
   EXPECT_EQ(*TEST_FLAG, 5.12);
}

// STRING_FLAGS //


TEST(StringFlags, OneSingleDashFlag) {
   constexpr int argc = 2;
   const char* argv[argc] = {"/some_directory", "-TESTING=WORKING"};

   const auto TEST_FLAG = flag::stringFlag("TESTING", "DEFAULT", "This is a flag");
   flag::parseFlags(argc, argv);
   EXPECT_EQ(*TEST_FLAG, "WORKING");
}