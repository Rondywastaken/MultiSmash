#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct {
  Vector2 pos;
  int w;
  int h;
  float speed;
  float velocity;
  Color color;
} Player;

extern void InitPlayer(Player *p, int player_type, int screen_w, int screen_h);
extern void DrawPlayer(Player *p);
extern void UpdatePlayer(Player *p, float deltaTime, int screen_w, int screen_h);

#endif 
