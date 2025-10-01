#ifndef SKINNER_FILEREADINGUTIL_H
#define SKINNER_FILEREADINGUTIL_H
#include <cassert>
#include <fstream>

#include "SkinningSession.h"
using std::optional;

inline std::vector<SkinningSession> read_sessions_file(const std::string &file_name) {
   std::ifstream input_stream(file_name);
   assert(input_stream.is_open());

   std::vector<SkinningSession> resultSessions{};
   optional<SkinningSession> new_session = SkinningSession::read_session_from_file(input_stream);

   while (new_session.has_value()) {
      resultSessions.push_back(new_session.value());
      new_session = SkinningSession::read_session_from_file(input_stream);
   }

   return resultSessions;
}

inline void write_sessions_to_file(const std::vector<SkinningSession> &sessions, const std::string &file_name) {
   std::ofstream output_stream(file_name);
   assert(output_stream.is_open());

   for (const auto &session: sessions) {
      session.write_session_to_file(output_stream);
   }
}


#endif //SKINNER_FILEREADINGUTIL_H
