#ifndef SKINNER_SKINNINGSESSION_H
#define SKINNER_SKINNINGSESSION_H
#include <chrono>
#include <fstream>

using std::chrono::seconds;
using std::optional;

struct SkinningSession {
   long start_time;
   long end_time;
   int breaks_taken;

   SkinningSession() = default;
   SkinningSession(long start_time, long end_time, int breaks_taken) : start_time(start_time), end_time(end_time),
                                                                       breaks_taken(breaks_taken) {
   }

   void write_session_to_file(std::ofstream &output_stream) const {
      output_stream << start_time << " " << end_time << " " << breaks_taken
            << std::endl;
   }

   static optional<SkinningSession> read_session_from_file(std::ifstream &input_stream) {
      SkinningSession new_session {};
      if (input_stream >> new_session.start_time >> new_session.end_time >> new_session.breaks_taken) {
         return new_session;
      }
      return optional<SkinningSession>();
   }
};

#endif //SKINNER_SKINNINGSESSION_H
