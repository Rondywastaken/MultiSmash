#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <raylib.h>

#include "connection.h"

#define WIDTH 1920
#define HEIGHT 1080
#define PLATFORM_WIDTH WIDTH*0.75
#define VELOCITY 10

#define BTN_WIDTH WIDTH*0.5

int showMenu = 1;
Color btnColor1 = LIME;
Color btnColor2 = LIGHTGRAY;
int activeBtn = 0;

int isHost = 0;

pthread_t client = 0;
pthread_t server = 0;

typedef struct Player {
  Vector2 pos;
  float speed;
  Color color;
} Player;

void CreatePlatform() {
  DrawRectangle(WIDTH/2-PLATFORM_WIDTH/2, HEIGHT/2+0.15*HEIGHT, PLATFORM_WIDTH, HEIGHT*0.1, GRAY);
}

void CreatePlayer(Player *player) {
  DrawRectangle(player->pos.x, player->pos.y, WIDTH*0.03, HEIGHT*0.1, player->color);
}

void MovePlayer(Player *player, float deltaTime) {
  if (player->pos.y <= HEIGHT/2+WIDTH*0.03) {
    player->pos.y += player->speed*deltaTime;
    player->speed += VELOCITY;
  }
  if (IsKeyDown(KEY_D)) {
    player->pos.x += player->speed*deltaTime;
  } else if (IsKeyDown(KEY_A)) {
    player->pos.x -= player->speed*deltaTime; 
  }
}

void CreateMenu() {
  DrawRectangle(WIDTH/2-BTN_WIDTH/2, HEIGHT/2-0.15*HEIGHT, BTN_WIDTH, HEIGHT*0.1, btnColor1);
  DrawText("HOST GAME", WIDTH/2-BTN_WIDTH/2, HEIGHT/2-0.15*HEIGHT, 100, MAROON);
  DrawRectangle(WIDTH/2-BTN_WIDTH/2, HEIGHT/2, BTN_WIDTH, HEIGHT*0.1, btnColor2);
  DrawText("JOIN HOST", WIDTH/2-BTN_WIDTH/2, HEIGHT/2, 100, MAROON);
}

void ActivateMenu(Vector2 *pos1, Vector2 *pos2) {
  if (IsKeyPressed(KEY_UP)) {
    btnColor1 = LIME;
    btnColor2 = LIGHTGRAY;
    activeBtn = 0;
  } else if (IsKeyPressed(KEY_DOWN)) {
    btnColor2 = LIME;
    btnColor1 = LIGHTGRAY;
    activeBtn = 1;
  }
  if (IsKeyPressed(KEY_ENTER)) {
    if (activeBtn == 0) {
      isHost = 1; 
      showMenu = 0;
      pthread_create(&server, NULL, server_thread, (void*)pos1);
      pthread_detach(server);
    } else {
      showMenu = 0;
      pthread_create(&client, NULL, client_thread, (void*)pos2);
      pthread_detach(client);
    }
    if (isHost) printf("You are the host\n");
    else printf("Joining host...\n");
  }
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "MultiSmash");
  SetTargetFPS(120);

  Player player1 = {0};
  player1.pos.x = WIDTH/2;
  player1.pos.y = HEIGHT/8;
  player1.speed = 0.1f;
  player1.color = RED;

  Player player2 = {0};
  player2.pos.x = WIDTH/2+500;
  player2.pos.y = HEIGHT/8;
  player2.speed = 0.1f;
  player2.color = GREEN;

  while (!WindowShouldClose()) {
    BeginDrawing();
      float deltaTime = GetFrameTime();
      DrawFPS(10, 10);
      ClearBackground(WHITE);
      if (showMenu) {
        CreateMenu(); 
        ActivateMenu(&player1.pos, &player2.pos);
      } else {
        // game
        CreatePlatform();
        CreatePlayer(&player1);
        CreatePlayer(&player2);
        if (isHost) {
          MovePlayer(&player1, deltaTime);
          pthread_mutex_lock(&move_mutex);
          player2.pos = server_move.pos;
          pthread_mutex_unlock(&move_mutex);
        } else {
          MovePlayer(&player2, deltaTime);
          pthread_mutex_lock(&move_mutex);
          player1.pos = client_move.pos;
          pthread_mutex_unlock(&move_mutex);
        }
      }
    EndDrawing();
  }

  if (server != 0) pthread_join(server, NULL);
  if (client != 0) pthread_join(client, NULL);

  CloseWindow();

  return EXIT_SUCCESS;
}
