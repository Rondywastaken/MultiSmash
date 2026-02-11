#include "platform.h"

Platform main_platform;

void InitPlatform(int screen_w, int screen_h) {
  main_platform.w = screen_w * 0.75f;
  main_platform.h = screen_h * 0.1f;
  main_platform.pos.x = screen_w / 2.0f - main_platform.w / 2.0f;
  main_platform.pos.y = screen_h / 2.0f + 0.15f * screen_h;
  main_platform.color = GRAY;
}

void DrawPlatform(void) {
  DrawRectangle(main_platform.pos.x, main_platform.pos.y, main_platform.w, main_platform.h, main_platform.color);
}
