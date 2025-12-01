#ifndef SKINNER_WORKVISUALISER_H
#define SKINNER_WORKVISUALISER_H

#include <vector>
#include "raylib.h"
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
      InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "HELLO");
   }

   void Render() {
      if (auto err = LoadArchivedSessions()) {
         std::string errorString = std::format("Failed to load archived sessions with error: {}", err->what());
         colours::PRINT(errorString.c_str(), colours::Red);
         std::exit(1);
      }

      while (!WindowShouldClose()) {
         BeginDrawing();
         ClearBackground(RAYWHITE);
         DrawBarGraph();
         DrawStatsText();
         EndDrawing();
      }
   }

   void DrawBarGraph();
   void DrawStatsText();


   ~WorkVisualiser() {
      CloseWindow();
   }

private:
   std::vector<std::pair<SkinningSession, std::chrono::year_month_day> > all_sessions_and_dates{};
   std::runtime_error* LoadArchivedSessions();

   static constexpr int CHART_WIDTH = SCREEN_WIDTH - 20;
   static constexpr int CHART_HEIGHT = SCREEN_HEIGHT - 100;
};


#endif //SKINNER_WORKVISUALISER_H
