#include "player.h"

void InitPlayer(Player *p, int player_type, int screen_w, int screen_h) {
  switch (player_type) {
    case 1:
      p->w = screen_w * 0.03f;
      p->h = screen_h * 0.1f;
      p->pos.x = screen_w / 2.0f;
      p->pos.y = screen_h / 8.0f;
      p->speed = 0.1f;
      p->velocity = 10.0f;
      p->color = RED;
      break;
    case 2:
      p->w = screen_w * 0.03f;
      p->h = screen_h * 0.1f;
      p->pos.x = screen_w / 2.0f + 500;
      p->pos.y = screen_h / 8.0f;
      p->speed = 0.1f;
      p->velocity = 10.0f;
      p->color = GREEN;
      break;
  }
}

void DrawPlayer(Player *p) {
  DrawRectangle(p->pos.x, p->pos.y, p->w, p->h, p->color);
}

void UpdatePlayer(Player *p, float deltaTime, int screen_w, int screen_h) {
  if (p->pos.y <= screen_h / 2.0f + screen_w * 0.03) {
    p->pos.y += p->speed*deltaTime;
    p->speed += p->velocity;
  }
  if (IsKeyDown(KEY_D)) {
    p->pos.x += p->speed*deltaTime;
  } else if (IsKeyDown(KEY_A)) {
    p->pos.x -= p->speed*deltaTime; 
  }
}

