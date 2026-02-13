#ifndef MENU_H
#define MENU_H

#include <pthread.h>
#include "raylib.h"
#include "connection.h"

typedef struct {
  Vector2 pos;
  int w;
  int h;
  Color color;
} Button;

typedef struct {
  Vector2 pos;
  Color color;
  int font_size;
} Text;

extern void InitMenu(int screen_w, int screen_h);
extern void DrawMenu(void);
extern Connection UpdateMenu(void);
extern char *GetIP(void);

#endif
