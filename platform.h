#ifndef PLATFORM_H
#define PLATFORM_H

#include "raylib.h"

typedef struct {
  Vector2 pos;
  int w;
  int h;
  Color color;
} Platform;

extern void InitPlatform(int screen_w, int screen_h);
extern void DrawPlatform(void);

#endif

