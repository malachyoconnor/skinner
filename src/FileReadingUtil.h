#ifndef SKINNER_FILEREADINGUTIL_H
#define SKINNER_FILEREADINGUTIL_H
#include <cassert>
#include <filesystem>
#include <fstream>

#include "SkinningInterval.h"
#include "SkinningSession.h"
using std::optional;
using std::runtime_error;

inline std::pair<SkinningSession *, runtime_error*> read_skinning_session(const std::string &file_name) {
   if (!std::filesystem::exists(file_name)) {
      return {nullptr, new runtime_error{"File does not exist."}};
   }

   std::ifstream input_stream(file_name);
   assert(input_stream.is_open());

   std::vector<SkinningInterval> resultSessions{};
   optional<SkinningInterval> new_session = SkinningInterval::read_interval_from_file(input_stream);

   while (new_session.has_value()) {
      resultSessions.push_back(new_session.value());
      new_session = SkinningInterval::read_interval_from_file(input_stream);
   }

   auto resultSession = new SkinningSession(resultSessions);

   return {resultSession, nullptr};
}

inline void write_session_to_file(const SkinningSession sessions, const std::string &file_name) {
   std::ofstream output_stream(file_name);
   assert(output_stream.is_open());

   for (const auto &session: sessions.session_log()) {
      session.write_interval_to_file(output_stream);
   }
}

#endif //SKINNER_FILEREADINGUTIL_H
