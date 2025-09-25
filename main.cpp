#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <utility>
#include <filesystem>
#include <random>

using namespace std;

enum Command {
   START,
   CHECK,
   count_
};

uniform_real_distribution<double> dist;


const array commands = {START, CHECK};
static_assert(commands.size() == count_);

bool operator==(Command lhs, string rhs) {
   if (rhs.empty()) return false;

   transform(rhs.begin(), rhs.end(), rhs.begin(),
             [](unsigned char c) { return std::tolower(c); });

   // TODO: Can we static assert that this is all the possibilities? Don't want default
   switch (lhs) {
      case START: return rhs == "start";
      case CHECK: return rhs == "check";

      default: return false;
   }
}

bool operator==(string lhs, Command rhs) {
   return rhs == std::move(lhs);
}

bool write_seconds_since_epoch(chrono::duration<long> seconds_since_epoch, int breaks_taken=0) {
   ofstream output("./start.txt");
   assert(output.is_open());

   output << seconds_since_epoch.count() << endl;
   output << breaks_taken << endl;

   output.flush();
   output.close();

   // TODO: Could do with some testing here

   return true;
}


int handleStart() {
   auto current_time_since_epoch = chrono::system_clock::now().time_since_epoch();
   auto seconds_since_epoch = chrono::duration_cast<chrono::seconds>(current_time_since_epoch);

   write_seconds_since_epoch(seconds_since_epoch);
   return 0;
}

int handleCheck() {
   ifstream input_stream("./start.txt");
   assert(input_stream.is_open());

   long long seconds_since_epoch;
   input_stream >> seconds_since_epoch;

   int breaks_completed;
   input_stream >> breaks_completed;

   chrono::duration<long> epoch_seconds(seconds_since_epoch);
   chrono::system_clock::time_point start_time(epoch_seconds);

   auto seconds_since_start =
         chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - start_time);


   auto seconds_since_last_break = seconds_since_start;

   std::mt19937 gen(seconds_since_epoch);
   // std::uniform_real_distribution<> dis(1.0, 2.0);
   std::uniform_real_distribution<> dis(0.001, 0.002);

   printf("We read this as your breaks completed %d\n", breaks_completed);


   int breaks_allowed = -breaks_completed;

   while (seconds_since_last_break.count() > 0) {
      double generated_hours_to_work = dis(gen);
      chrono::duration<long> work_seconds(static_cast<long>(generated_hours_to_work * 3600));

      seconds_since_last_break -= work_seconds;
      if (seconds_since_last_break.count() > 0) {
         breaks_allowed++;
      }
   }

   if (breaks_allowed > 0) {
      printf("\033[1;32mYou may have a break\033[0m\n");
      printf("\033[1;32mShould you choose to accept one\033[0m\n");
      getchar();

      write_seconds_since_epoch(chrono::duration<long>(seconds_since_epoch), breaks_completed + 1);

   } else {
      printf("\033[1;31mGet back to work mutant\033[0m\n");
   }







   return 0;
}


int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("Incorrect number of commands entered. Put either 'start' or 'check'.\n");
   }

   Command relevantCommand = count_;

   for (auto command: commands) {
      if (argv[1] == command) {
         relevantCommand = command;
      }
   }

   if (relevantCommand == count_) {
      printf("Incorrect command, enter 'start' or 'check'.");
      return 0;
   }

   switch (relevantCommand) {
      case START: return handleStart();
      case CHECK: return handleCheck();

      default: {
         printf("Encountered %d. what...", relevantCommand);
         return -1;
      }
   }

   return 0;
}
