#include "WorkVisualiser.h"

#include <filesystem>
#include <iostream>

#include "FileReadingUtil.h"

runtime_error *WorkVisualiser::LoadArchivedSessions() {
   all_sessions_and_dates.clear();

   for (const auto &dir: std::filesystem::directory_iterator(ARCHIVE_LOCATION)) {
      if (dir.is_directory()) continue;
      if (!(dir.path().has_extension() && dir.path().extension() == ".txt")) continue;

      auto [pastSession, err] = read_session_from_file(dir.path());

      if (err) return err;
      if (pastSession->get_interval_list().empty()) continue;

      std::stringstream ss{dir.path().filename().string()};
      std::chrono::year_month_day ymd{};
      ss >> std::chrono::parse("%F", ymd);

      if (ss.fail()) {
         std::string error_text = std::format("Failed to parse date string: {}", dir.path().string());
         return new runtime_error(error_text);
      }

      all_sessions_and_dates.emplace_back(*pastSession, ymd);
   }

   if (all_sessions_and_dates.empty()) return new runtime_error("No archived sessions");

   std::sort(all_sessions_and_dates.begin(), all_sessions_and_dates.end(),
             [](const std::pair<SkinningSession, std::chrono::year_month_day> &p1,
                const std::pair<SkinningSession, std::chrono::year_month_day> &p2) {
                return p1.second < p2.second;
             });

   return nullptr;
}

void WorkVisualiser::DrawBarGraph() {
   constexpr int BAR_WIDTH = 5;

   Raylib::DrawRectangle(10, 10, BAR_WIDTH, CHART_HEIGHT - 20, Raylib::Black);
   Raylib::DrawRectangle(10, CHART_HEIGHT - 10, CHART_WIDTH, BAR_WIDTH, Raylib::Black);

   std::chrono::sys_days first_day = all_sessions_and_dates.front().second;
   std::chrono::sys_days last_day = all_sessions_and_dates.back().second;

   const int number_of_bars = (last_day - first_day).count() + 1;

   const int BAR_GAP = (CHART_WIDTH - BAR_WIDTH * number_of_bars) / (number_of_bars - 1);

   const int firstBarX = 10 + 5 + BAR_GAP;
   constexpr int firstBarY = CHART_HEIGHT - 10;

   constexpr int barHeightPerHour = CHART_HEIGHT / 12;

   for (auto [session, date]: all_sessions_and_dates) {

      int bar_index = (std::chrono::sys_days(date) - first_day).count();
      const int barX = firstBarX + bar_index * (BAR_WIDTH + BAR_GAP);

      const auto hoursWorked = session.CalculateTotalWorkTime() / 3600;
      assert(hoursWorked < 12 && "You worked more than 12 hours???");

      const int barHeight = barHeightPerHour * hoursWorked;

      Raylib::DrawRectangle(barX, firstBarY - barHeight, BAR_WIDTH, barHeight, Raylib::Red);
   }
}

void WorkVisualiser::DrawAllText() {
   DrawStatsText();
}

void WorkVisualiser::DrawStatsText() {

   Raylib::DrawRectangle(10, CHART_HEIGHT + 10, CHART_WIDTH, SCREEN_HEIGHT - CHART_HEIGHT - 20, Raylib::Gray);

   double totalHoursWorked = 0;
   double totalIntervals = 0;
   int number_of_sessions = all_sessions_and_dates.size();

   for (auto [session, _]: all_sessions_and_dates) {
      totalHoursWorked += session.CalculateTotalWorkTime() / 3600.0;
      totalIntervals += session.get_num_intervals();
   }

   std::string avg_hours_worked = std::format("Average hours worked: {:.2}", totalHoursWorked / number_of_sessions);
   std::string avg_number_of_sessions = std::format("Average num intervals   : {:.2}",
                                                    totalIntervals / number_of_sessions);

   Raylib::DrawText(avg_hours_worked.c_str(), 20, CHART_HEIGHT + 20, 30, Raylib::Black);
   Raylib::DrawText(avg_number_of_sessions.c_str(), 20, CHART_HEIGHT + 50, 30, Raylib::Black);

}

void WorkVisualiser::DrawHoverText() {

}
