#ifndef SKINNER_SKINNINGSESSIONLOG_H
#define SKINNER_SKINNINGSESSIONLOG_H
#include <chrono>

enum SessionState {
   EMPTY,
   PAUSED,
   IN_PROGRESS,
};

class SkinningSession {
public:
   SkinningSession() = delete;

   explicit SkinningSession(std::vector<SkinningInterval> sessions) {
      _session_log = std::move(sessions);
   }

   [[nodiscard]] SessionState session_state() const {
      return calculate_session_state();
   }

   [[nodiscard]] std::vector<SkinningInterval> &session_log() {
      return _session_log;
   }

   [[nodiscard]] std::vector<SkinningInterval> session_log() const {
      return _session_log;
   }

   static SkinningSession *newSkinningSession() {
      std::vector<SkinningInterval> empty_session_log{};
      return new SkinningSession(empty_session_log);
   }

private:

   [[nodiscard]] SessionState calculate_session_state() const {
      if (_session_log.empty())
         return EMPTY;

      if (_session_log.back().end_time != -1)
         return PAUSED;

      if (_session_log.back().end_time == -1)
         return IN_PROGRESS;

      throw std::logic_error("Session state could not be calculated!");
   }

   std::vector<SkinningInterval> _session_log;
};

#endif
