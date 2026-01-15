#ifndef SKINNER_WORKVISUALISER_H
#define SKINNER_WORKVISUALISER_H

#include <vector>
#include "raylib_namespaced.h"
#include "SkinningSession.h"
#include "utils.h"

constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 600;

// Want a big bar graph.
// Want average session length
// Average number of breaks
// Average NUMBER of skinning sessions

class WorkVisualiser {
public:
   WorkVisualiser() {
      Raylib::InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "HELLO");
   }

   void Render() {
      if (const auto err = LoadArchivedSessions()) {
         const std::string errorString = std::format("Failed to load archived sessions with error: {}", err->what());
         colours::PRINT(errorString.c_str(), colours::Red);
         std::exit(1);
      }


      const std::chrono::sys_days first_day = all_sessions_and_dates.front().second;
      const std::chrono::sys_days last_day = all_sessions_and_dates.back().second;

      const int endSessionIndex = static_cast<int>((last_day - first_day).count() + 1);

      while (!Raylib::WindowShouldClose()) {

         Raylib::BeginDrawing();
         Raylib::ClearBackground(Raylib::RayWhite);
         DrawBarGraph(sliderIndex_, endSessionIndex);
         DrawAllText(sliderIndex_, endSessionIndex);
         DrawSlider(sliderIndex_, endSessionIndex);
         Raylib::EndDrawing();
      }
   }

   void DrawBarGraph(int firstDayIndex, int numberOfBars);
   void DrawSingleBar(int firstDayIndex, int barIndex, int numberOfBars, Color color);
   void DrawAllText(int firstDayIndex, int endDayIndex);

   ~WorkVisualiser() {
      Raylib::CloseWindow();
   }

private:
   bool MouseInsideSliderButton(int sliderX);
   void DrawSlider(int sliderIndex, int endSessionIndex);
   void DrawStatsText(int firstDayIndex, int endDayIndex);
   void DrawHover(int firstDayIndex, int lastDayIndex);
   static bool MouseInsideGraph();
   std::optional<SkinningSession> GetSessionFromBarIndex(int barIndex);
   int sliderIndex_ = 0;
   bool mouseWasClickingSlider_ = false;

   std::vector<std::pair<SkinningSession, std::chrono::year_month_day> > all_sessions_and_dates{};
   std::runtime_error *LoadArchivedSessions();

   static constexpr int CHART_WIDTH = SCREEN_WIDTH - 20;
   static constexpr int CHART_HEIGHT = SCREEN_HEIGHT - 100;
   static constexpr int BAR_WIDTH = 5;
   static constexpr int AXIS_WIDTH = 5;

   static constexpr int BAR_HEIGHT_PER_HOUR_WORKED = CHART_HEIGHT / 12;

   static constexpr int SLIDER_WIDTH = 150;
   static constexpr int SLIDER_HEIGHT = 50;
   static constexpr int SLIDER_X = CHART_WIDTH / 2 + CHART_WIDTH / 4 - SLIDER_WIDTH / 2;
   static constexpr int SLIDER_Y = CHART_HEIGHT + 10 + (SCREEN_HEIGHT - CHART_HEIGHT - 20) / 2 - SLIDER_HEIGHT/2;
   static constexpr int SLIDER_BUTTON_WIDTH = 30;
   static constexpr int SLIDER_BUTTON_HEIGHT = SLIDER_HEIGHT;
};


#endif //SKINNER_WORKVISUALISER_H
