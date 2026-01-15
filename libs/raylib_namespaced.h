#pragma once
#include "raylib.h"

namespace Raylib {
   using Vector2 = ::Vector2;
   using Color = ::Color;

   inline void InitWindow(const int width, const int height, const char *title) {
      ::InitWindow(width, height, title);
   }

   inline void CloseWindow() {
      ::CloseWindow(); // Close window and unload OpenGL context
   }

   inline bool WindowShouldClose() {
      return ::WindowShouldClose();
      // Check if application should close (KEY_ESCAPE pressed or windows close icon clicked)
   }

   inline void DrawRectangle(const int posX, const int posY, const int width, const int height, const Color color) {
      ::DrawRectangle(posX, posY, width, height, color);
   }

   inline void BeginDrawing() {
      ::BeginDrawing(); // Setup canvas (framebuffer) to start drawing
   }

   inline void EndDrawing() {
      ::EndDrawing(); // End canvas drawing and swap buffers (double buffering)
   }

   inline void ClearBackground(const Color color) {
      ::ClearBackground(color);
   }

   inline void DrawText(const char *text, const int posX, const int posY, const int fontSize, const Color color) {
      ::DrawText(text, posX, posY, fontSize, color); // Draw text (using default font)
   }


   inline int GetMouseX(void) {
      return ::GetMouseX(); // Get mouse position X
   }

   inline int GetMouseY(void) {
      return ::GetMouseY(); // Get mouse position Y
   }

   inline int MeasureText(const char *text, const int fontSize) {
      return ::MeasureText(text, fontSize); // Measure string width for default font
   }

   static constexpr Color LightGray = LIGHTGRAY;
   static constexpr Color Gray = GRAY;
   static constexpr Color DarkGray = DARKGRAY;
   static constexpr Color Yellow = YELLOW;
   static constexpr Color Gold = GOLD;
   static constexpr Color Orange = ORANGE;
   static constexpr Color Pink = PINK;
   static constexpr Color Red = RED;
   static constexpr Color Maroon = MAROON;
   static constexpr Color Green = GREEN;
   static constexpr Color Lime = LIME;
   static constexpr Color Darkgreen = DARKGREEN;
   static constexpr Color Skyblue = SKYBLUE;
   static constexpr Color Blue = BLUE;
   static constexpr Color Darkblue = DARKBLUE;
   static constexpr Color Purple = PURPLE;
   static constexpr Color Violet = VIOLET;
   static constexpr Color DarkPurple = DARKPURPLE;
   static constexpr Color Beige = BEIGE;
   static constexpr Color Brown = BROWN;
   static constexpr Color Darkbrown = DARKBROWN;
   static constexpr Color White = WHITE;
   static constexpr Color Black = BLACK;
   static constexpr Color Blank = BLANK;
   static constexpr Color Magenta = MAGENTA;
   static constexpr Color RayWhite = RAYWHITE;
}
