#include "tile.h"

Tile main_tile;

void InitTile(int screen_w, int screen_h) {
  main_tile.w = screen_w * 0.75f;
  main_tile.h = screen_h * 0.1f;
  main_tile.pos.x = screen_w / 2.0f - main_tile.w / 2.0f;
  main_tile.pos.y = screen_h / 2.0f + 0.15f * screen_h;
  main_tile.color = GRAY;
}

void DrawTile(void) {
  DrawRectangle(main_tile.pos.x, main_tile.pos.y, main_tile.w, main_tile.h, main_tile.color);
}
