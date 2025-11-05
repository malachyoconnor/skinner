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
      _interval_list = std::move(sessions);
   }

   [[nodiscard]] SessionState session_state() const {
      return calculate_session_state();
   }

   [[nodiscard]] std::vector<SkinningInterval> &get_inteval_list() {
      return _interval_list;
   }

   [[nodiscard]] std::vector<SkinningInterval> get_inteval_list() const {
      return _interval_list;
   }

   static SkinningSession *newSkinningSession() {
      std::vector<SkinningInterval> empty_session_log{};
      return new SkinningSession(empty_session_log);
   }

private:

   [[nodiscard]] SessionState calculate_session_state() const {
      if (_interval_list.empty())
         return EMPTY;

      if (_interval_list.back().end_time != -1)
         return PAUSED;

      if (_interval_list.back().end_time == -1)
         return IN_PROGRESS;

      throw std::logic_error("Session state could not be calculated!");
   }

   std::vector<SkinningInterval> _interval_list;
};

#endif
