#ifndef SKINNER_FILEREADINGUTIL_H
#define SKINNER_FILEREADINGUTIL_H
#include <cassert>
#include <fstream>

#include "SkinningSession.h"

inline std::vector<SkinningSession> read_sessions_file(const std::string& file_name) {
   std::ifstream input_stream(file_name);
   assert(input_stream.is_open());

   std::vector<SkinningSession> resultSessions {};
   SkinningSession new_session {};

   while (input_stream >> new_session.start_time >> new_session.breaks_taken) {
      resultSessions.push_back(new_session);
   }

   return resultSessions;
}

inline void write_sessions_to_file(const std::vector<SkinningSession>& sessions, const std::string& file_name) {
   std::ofstream input_stream(file_name);
   assert(input_stream.is_open());

   for (const auto& session : sessions) {
      input_stream << session.start_time << " " << session.breaks_taken << std::endl;
   }
}


#endif //SKINNER_FILEREADINGUTIL_H