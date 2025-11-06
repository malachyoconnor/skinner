#ifndef SKINNER_SKINNINGSESSION_H
#define SKINNER_SKINNINGSESSION_H
#include <chrono>
#include <fstream>

using namespace std::chrono_literals;
using std::chrono::seconds, std::chrono::time_point, std::chrono::system_clock;
using std::optional;
using std::string;

struct SkinningInterval {
   long start_time;
   long end_time;
   int breaks_taken;

   SkinningInterval() = default;

   SkinningInterval(long start_time, long end_time, int breaks_taken) : start_time(start_time), end_time(end_time),
                                                                        breaks_taken(breaks_taken) {
   }

   void write_interval_to_file(std::ofstream &output_stream) const;

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
