#ifndef TILE_H
#define TILE_H

#include "raylib.h"

typedef struct {
  Vector2 pos;
  int w;
  int h;
  Color color;
} Tile;

extern void InitTile(int screen_w, int screen_h);
extern void DrawTile(void);

#endif

