#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "tile.h"
#include "connection.h"     
#include "menu.h"
#include "state_machine.h"
#include "player.h"

#define WIDTH 1920
#define HEIGHT 1080

Connection conn_type;
pthread_t guest = 0;
pthread_t host = 0;
pthread_t remote = 0;

GameState game_state = STATE_MENU;
Player player_1 = {0};
Player player_2 = {0};

int main(void) {
  #ifdef _WIN32
    struct WSAData data; 
    WSAStartup(MAKEWORD(2,2), &data);
  #endif 
  
  InitWindow(WIDTH, HEIGHT, "MultiSmash");
  SetTargetFPS(120);

  InitPlayer(&player_1, 1, WIDTH, HEIGHT);
  InitPlayer(&player_2, 2, WIDTH, HEIGHT);
  InitMenu(WIDTH, HEIGHT);
  InitTile(WIDTH, HEIGHT);

  while (!WindowShouldClose()) {
    BeginDrawing();
      float deltaTime = GetFrameTime();
      DrawFPS(10, 10);
      ClearBackground(WHITE);
      if (game_state == STATE_MENU) {
        DrawMenu(); 
        conn_type = UpdateMenu();
        if (conn_type == CONN_HOST) {
          pthread_create(&host, NULL, host_thread, (void*)&player_1.pos);
          pthread_detach(host);
        } else if (conn_type == CONN_GUEST) {
          pthread_create(&guest, NULL, guest_thread, (void*)&player_2.pos);
          pthread_detach(guest);
        } else if (conn_type == CONN_REMOTE) {
          pthread_create(&remote, NULL, client_thread, (void*)&player_1.pos);
          pthread_detach(remote);
        }
      } 
      else if (game_state == STATE_PLAYING) {
        // game
        DrawTile();
        DrawPlayer(&player_1);
        DrawPlayer(&player_2);
        if (conn_type == CONN_HOST) {
          UpdatePlayer(&player_1, deltaTime, WIDTH, HEIGHT);
          pthread_mutex_lock(&move_mutex);
          player_2.pos = host_move.pos;
          pthread_mutex_unlock(&move_mutex);
        } else if (conn_type == CONN_GUEST) {
          UpdatePlayer(&player_2, deltaTime, WIDTH, HEIGHT);
          pthread_mutex_lock(&move_mutex);
          player_1.pos = guest_move.pos;
          pthread_mutex_unlock(&move_mutex);
        } else if (conn_type == CONN_REMOTE) {
          UpdatePlayer(&player_1, deltaTime, WIDTH, HEIGHT);
          pthread_mutex_lock(&move_mutex);
          player_2.pos = remote_move.pos;
          pthread_mutex_unlock(&move_mutex);
        }
      }
    EndDrawing();
  }

  if (host != 0) pthread_join(host, NULL);
  if (guest != 0) pthread_join(guest, NULL);
  if (remote != 0) pthread_join(remote, NULL);

  CloseWindow();

  return EXIT_SUCCESS;
}
