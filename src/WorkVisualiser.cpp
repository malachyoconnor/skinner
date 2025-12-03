#include "WorkVisualiser.h"

#include <filesystem>
#include <iostream>
#include <ranges>

#include "FileReadingUtil.h"

void WorkVisualiser::DrawBarGraph(int firstDayIndex, int endSessionIndex) {
   Raylib::DrawRectangle(10, 10, AXIS_WIDTH, CHART_HEIGHT - 20, Raylib::Black);
   Raylib::DrawRectangle(10, CHART_HEIGHT - 10, CHART_WIDTH, AXIS_WIDTH, Raylib::Black);

   for (int barIndex = firstDayIndex; barIndex < endSessionIndex; barIndex++) {
      if (!GetSessionFromBarIndex(barIndex)) continue;

      DrawSingleBar(firstDayIndex, barIndex, (endSessionIndex - firstDayIndex), Raylib::Red);
   }
}

void WorkVisualiser::DrawSingleBar(int firstDayIndex, int barIndex, int numberOfBars, Raylib::Color color) {
   const int BAR_GAP = (CHART_WIDTH - BAR_WIDTH * numberOfBars) / (numberOfBars - 1);
   const int barX = 10 + AXIS_WIDTH + BAR_GAP + (barIndex - firstDayIndex) * (BAR_WIDTH + BAR_GAP);
   constexpr int firstBarY = CHART_HEIGHT - 10;

   SkinningSession session = GetSessionFromBarIndex(barIndex).value();
   const auto hoursWorked = session.CalculateTotalWorkTime() / 3600;
   assert(hoursWorked < 12 && "You worked more than 12 hours???");
   const int barHeight = BAR_HEIGHT_PER_HOUR_WORKED * hoursWorked;

   Raylib::DrawRectangle(barX, firstBarY - barHeight, BAR_WIDTH, barHeight, color);
}

void WorkVisualiser::DrawAllText(int firstDayIndex, int endDayIndex) {
   DrawStatsText(firstDayIndex, endDayIndex);
   DrawHover(firstDayIndex, endDayIndex);
}

void WorkVisualiser::DrawSlider(int sliderIndex, int endSessionIndex) {

   DrawRectangle(SLIDER_X, SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT, Raylib::Black);

   double sliderPercentage = sliderIndex / static_cast<double>(endSessionIndex);
   const int sliderX = SLIDER_X + static_cast<int>(sliderPercentage * SLIDER_WIDTH);

   DrawRectangle(sliderX, SLIDER_Y, SLIDER_BUTTON_WIDTH, SLIDER_BUTTON_HEIGHT, Raylib::Red);

   int mouseX = Raylib::GetMouseX();
   if ((mouseWasClickingSlider_ || MouseInsideSliderButton(sliderX)) && IsMouseButtonDown(0)) {
      mouseWasClickingSlider_ = true;

      // Try to move the center of the slider to the mouse
      const int newSliderX = mouseX - SLIDER_BUTTON_WIDTH / 2;

      if (newSliderX < SLIDER_X) {
         sliderIndex_ = 0;
         return;
      }
      if (newSliderX > SLIDER_X + SLIDER_WIDTH) {
         sliderIndex_ = endSessionIndex - 1;
         return;
      }

      sliderIndex_ = static_cast<int>((mouseX - SLIDER_X - 1 - SLIDER_BUTTON_WIDTH / 2) / static_cast<double>(
                                         SLIDER_WIDTH) * (
                                         endSessionIndex - 1));
   } else {
      mouseWasClickingSlider_ = false;
   }
}

bool WorkVisualiser::MouseInsideSliderButton(int sliderX) {
   int mouseX = Raylib::GetMouseX();
   int mouseY = Raylib::GetMouseY();

   return mouseX >= sliderX && mouseX < sliderX + SLIDER_WIDTH
          && mouseY >= SLIDER_Y && mouseY < SLIDER_Y + SLIDER_HEIGHT;

}

void WorkVisualiser::DrawStatsText(int firstDayIndex, int endDayIndex) {
   double totalHoursWorked = 0;
   double totalIntervals = 0;
   int numberOfSessionsWhereSkinnerWasActive = 0;

   Raylib::DrawRectangle(10, CHART_HEIGHT + 10, CHART_WIDTH, SCREEN_HEIGHT - CHART_HEIGHT - 20, Raylib::Gray);

   for (int sessionIndex = firstDayIndex; sessionIndex < endDayIndex; ++sessionIndex) {
      if (!GetSessionFromBarIndex(sessionIndex)) continue;
      SkinningSession session = GetSessionFromBarIndex(sessionIndex).value();

      totalHoursWorked += session.CalculateTotalWorkTime() / 3600.0;
      totalIntervals += session.GetNumberOfIntervals();
      numberOfSessionsWhereSkinnerWasActive += 1;
   }

   std::string avg_hours_worked = std::format("Average hours worked: {:.2}",
                                              totalHoursWorked / numberOfSessionsWhereSkinnerWasActive);
   std::string avg_number_of_sessions = std::format("Average num intervals   : {:.2}",
                                                    totalIntervals / numberOfSessionsWhereSkinnerWasActive);

   Raylib::DrawText(avg_hours_worked.c_str(), 20, CHART_HEIGHT + 20, 30, Raylib::Black);
   Raylib::DrawText(avg_number_of_sessions.c_str(), 20, CHART_HEIGHT + 50, 30, Raylib::Black);

}

void WorkVisualiser::DrawHover(int firstDayIndex, int lastDayIndex) {
   if (!MouseInsideGraph()) return;

   const int numberOfBars = lastDayIndex - firstDayIndex;
   const int BAR_GAP = (CHART_WIDTH - BAR_WIDTH * numberOfBars) / (numberOfBars - 1);
   const int mouseX = Raylib::GetMouseX();
   const int mouseY = Raylib::GetMouseY();

   const int ChartX = 10 + AXIS_WIDTH;
   const bool mouseOnABar = ((mouseX - ChartX) % (BAR_GAP + BAR_WIDTH)) > (BAR_GAP / 2);

   const char *ON_BAR = mouseOnABar ? "MOUSE ON BAR" : "";
   Raylib::DrawText(ON_BAR, 50, 50, 50, Raylib::Green);

   if (!mouseOnABar) return;

   const int barIndex = (mouseX - ChartX) / (BAR_GAP + BAR_WIDTH) + firstDayIndex;
   if (!GetSessionFromBarIndex(barIndex)) return;
   DrawSingleBar(firstDayIndex, barIndex, numberOfBars, Raylib::Blue);

   SkinningSession session = GetSessionFromBarIndex(barIndex).value();
   const std::string workText = std::format("Worked for {:.2} hours",
                                            static_cast<double>(session.CalculateTotalWorkTime()) / 3600.0);
   const std::string breakText = std::format("Relaxed for {:.2} hours",
                                             static_cast<double>(session.CalculateTotalBreakTime()) / 3600.0);

   constexpr int boxPadding = 10;
   constexpr int fontSize = 25;
   constexpr int boxHeight = boxPadding * 2 + fontSize * 2;
   const int boxWidth = boxPadding * 2 + std::max(Raylib::MeasureText(workText.c_str(), fontSize),
                                                  Raylib::MeasureText(breakText.c_str(), fontSize));

   int boxX = mouseX + 10;
   if (boxX + boxWidth > SCREEN_WIDTH) {
      boxX = SCREEN_WIDTH - boxWidth - 5;
   }
   int textX = boxX + boxPadding;

   Raylib::DrawRectangle(boxX, mouseY + 10, boxWidth, boxHeight, GRAY);
   Raylib::DrawText(workText.c_str(), textX, mouseY + 10 + boxPadding, fontSize, Raylib::Black);
   Raylib::DrawText(breakText.c_str(), textX, mouseY + 10 + boxPadding + fontSize, fontSize,
                    Raylib::Black);
}

bool WorkVisualiser::MouseInsideGraph() {
   const int mouseX = Raylib::GetMouseX();
   const int mouseY = Raylib::GetMouseY();

   return mouseX >= 10 && mouseX < 10 + CHART_WIDTH
          && mouseY >= 10 && mouseY < 10 + CHART_HEIGHT;
}

std::optional<SkinningSession> WorkVisualiser::GetSessionFromBarIndex(int barIndex) {
   std::chrono::sys_days first_day = all_sessions_and_dates.front().second;

   for (auto &[session, date]: all_sessions_and_dates) {
      int sessionIndex = (std::chrono::sys_days(date) - first_day).count();
      if (sessionIndex == barIndex) return session;
   }

   return std::nullopt;
}

runtime_error *WorkVisualiser::LoadArchivedSessions() {
   all_sessions_and_dates.clear();

   for (const auto &dir: std::filesystem::directory_iterator(ARCHIVE_LOCATION)) {
      if (dir.is_directory()) continue;
      if (!(dir.path().has_extension() && dir.path().extension() == ".txt")) continue;

      auto [pastSession, err] = read_session_from_file(dir.path());

      if (err) return err;
      if (pastSession->GetIntervalList().empty()) continue;

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
