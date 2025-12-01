#ifndef SKINNER_SKINNINGSESSIONLOG_H
#define SKINNER_SKINNINGSESSIONLOG_H
#include <chrono>

#include "SkinningInterval.h"
#include "utils.h"

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

   [[nodiscard]] std::size_t get_num_intervals() const {
      return _interval_list.size();
   }

   [[nodiscard]] std::vector<SkinningInterval>& get_interval_list() {
      return _interval_list;
   }

   [[nodiscard]] std::vector<SkinningInterval> get_interval_list() const {
      return _interval_list;
   }

   static SkinningSession *newSkinningSession() {
      std::vector<SkinningInterval> empty_session_log{};
      return new SkinningSession(empty_session_log);
   }

   long CalculateTotalWorkTime() const {
      long work_time = 0;

      for (SkinningInterval interval: get_interval_list()) {
         if (interval.end_time == -1) {
            interval.end_time = get_seconds_since_epoch();
         }

         work_time += interval.end_time - interval.start_time;
      }

      return work_time;
   }

   long CalculateTotalBreakTime() const {
      long break_time = 0;
      long break_start_time = get_interval_list().front().start_time;

      for (SkinningInterval interval: get_interval_list()) {
         if (interval.end_time == -1) {
            interval.end_time = get_seconds_since_epoch();
         }

         break_time += interval.start_time - break_start_time;
         break_start_time = interval.end_time;
      }

      return break_time;
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
