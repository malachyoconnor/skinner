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
      if (auto err = LoadArchivedSessions()) {
         std::string errorString = std::format("Failed to load archived sessions with error: {}", err->what());
         colours::PRINT(errorString.c_str(), colours::Red);
         std::exit(1);
      }


      std::chrono::sys_days first_day = all_sessions_and_dates.front().second;
      std::chrono::sys_days last_day = all_sessions_and_dates.back().second;
      const int numberOfBars = static_cast<int>((last_day - first_day).count() + 1);

      while (!Raylib::WindowShouldClose()) {
         Raylib::BeginDrawing();
         Raylib::ClearBackground(Raylib::RayWhite);
         DrawBarGraph(numberOfBars);
         DrawAllText(numberOfBars);
         Raylib::EndDrawing();
      }
   }

   void DrawBarGraph(int numberOfBars);
   void DrawSingleBar(int barIndex, int numberOfBars, Raylib::Color color);
   void DrawAllText(int numberOfBars);

   ~WorkVisualiser() {
      Raylib::CloseWindow();
   }

private:
   void DrawStatsText();
   void DrawHover(int numberOfBars);
   static bool MouseInsideGraph();
   std::optional<SkinningSession> GetSessionFromBarIndex(int barIndex);

   std::vector<std::pair<SkinningSession, std::chrono::year_month_day> > all_sessions_and_dates{};
   std::runtime_error *LoadArchivedSessions();

   static constexpr int CHART_WIDTH = SCREEN_WIDTH - 20;
   static constexpr int CHART_HEIGHT = SCREEN_HEIGHT - 100;
   static constexpr int BAR_WIDTH = 5;
   static constexpr int AXIS_WIDTH = 5;

   static constexpr int BAR_HEIGHT_PER_HOUR_WORKED = CHART_HEIGHT / 12;
};


#endif //SKINNER_WORKVISUALISER_H
