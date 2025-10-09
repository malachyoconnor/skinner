#ifndef SKINNER_SKINNINGSESSION_H
#define SKINNER_SKINNINGSESSION_H
#include <chrono>
#include <fstream>

using std::chrono::seconds;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::optional;
using std::string;
using namespace std::chrono_literals;

struct SkinningInterval {
   long start_time;
   long end_time;
   int breaks_taken;

   SkinningInterval() = default;

   SkinningInterval(long start_time, long end_time, int breaks_taken) : start_time(start_time), end_time(end_time),
                                                                       breaks_taken(breaks_taken) {
   }

   void write_interval_to_file(std::ofstream &output_stream) const {
      // Pretty printing human-readable time into file
      auto start_time_point = system_clock::from_time_t(start_time) + 1h;
      string human_readable_time = std::format("{0:%R}", start_time_point);

      if (end_time != -1) {
         auto end_time_point = system_clock::from_time_t(end_time) + 1h;
         human_readable_time += std::format("<-->{0:%R}", end_time_point);
      }

      output_stream << human_readable_time << " "<< start_time << " " << end_time << " " << breaks_taken
            << std::endl;
   }

   static optional<SkinningInterval> read_interval_from_file(std::ifstream &input_stream) {
      SkinningInterval new_session{};
      std::string pretty_printed_time{};

      if (input_stream >> pretty_printed_time >> new_session.start_time >> new_session.end_time >> new_session.
          breaks_taken) {
         return new_session;
      }
      return std::nullopt;
   }
};

#endif //SKINNER_SKINNINGSESSION_H
