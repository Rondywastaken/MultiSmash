#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "connection.h"     
#include "menu.h"
#include "state_machine.h"

#define WIDTH 1920
#define HEIGHT 1080
#define PLATFORM_WIDTH WIDTH*0.75
#define VELOCITY 10

int showMenu = 1;
Color btnColor1 = LIME;
Color btnColor2 = LIGHTGRAY;
int activeBtn = 0;

Connection conn_type;
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

GameState game_state = STATE_MENU;

int main(void) {
  #ifdef _WIN32
    struct WSAData data; 
    WSAStartup(MAKEWORD(2,2), &data);
  #endif 
  
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

  InitMenu(WIDTH, HEIGHT);

  while (!WindowShouldClose()) {
    BeginDrawing();
      float deltaTime = GetFrameTime();
      DrawFPS(10, 10);
      ClearBackground(WHITE);
      if (game_state == STATE_MENU) {
        DrawMenu(); 
        conn_type = UpdateMenu();
        if (conn_type == CONN_SERVER) {
          pthread_create(&server, NULL, server_thread, (void*)&player1.pos);
          pthread_detach(server);
        } else if (conn_type == CONN_CLIENT) {
          pthread_create(&client, NULL, client_thread, (void*)&player2.pos);
          pthread_detach(client);
        } 
      } 
      else if (game_state == STATE_PLAYING) {
        // game
        CreatePlatform();
        CreatePlayer(&player1);
        CreatePlayer(&player2);
        if (conn_type == CONN_SERVER) {
          MovePlayer(&player1, deltaTime);
          pthread_mutex_lock(&move_mutex);
          player2.pos = server_move.pos;
          pthread_mutex_unlock(&move_mutex);
        } else if (conn_type == CONN_CLIENT) {
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
