#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "raylib.h"
#include "connection.h"

MovePacket server_move = {0};
MovePacket client_move = {0};

pthread_mutex_t move_mutex = PTHREAD_MUTEX_INITIALIZER;

void *server_thread(void* arg) {
  Vector2 *pos = (Vector2*)arg;

  int server_fd;
  struct sockaddr_in server_addr, client_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  socklen_t server_addr_len = sizeof(server_addr);
  socklen_t client_addr_len = sizeof(client_addr);

  // Creating server socket
  server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_fd < 0) {
    perror("Server socket failed to create");
    exit(EXIT_FAILURE);
  }

  // bind socket to port 8080 for now 
  if (bind(server_fd, (struct sockaddr*)&server_addr, server_addr_len) < 0) {
    perror("Failed to bind socket");
    exit(EXIT_FAILURE);
  } 

  printf("Server listening on port 8080\n");

  while (1) {
    MovePacket incoming_pkt = {0};

    // Receive messages
    int msg_len = recvfrom(server_fd, (char*)&incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (msg_len == 0) {
      printf("No message was able to be received\n");
    } else if (msg_len < 0) {
      perror("Failed to receive message");
      exit(EXIT_FAILURE);
    }
    //printf("Received message of size %d\n", msg_len);

    pthread_mutex_lock(&move_mutex);
    server_move.pos = incoming_pkt.pos;
    pthread_mutex_unlock(&move_mutex);

    MovePacket move = {0};

    pthread_mutex_lock(&move_mutex);
    move.pos = *pos;
    pthread_mutex_unlock(&move_mutex);

    sendto(server_fd, (char*)&move, sizeof(move), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
  }
  
  #ifdef _WIN32
    closesocket(server_fd);
    WSACleanup();
  #else
    close(server_fd);
  #endif
  
  return NULL;
}

void *client_thread(void *arg) {
  Vector2 *pos = (Vector2*)arg;

  int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in server_addr, client_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);

  //inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  inet_pton(AF_INET, "10.179.255.109", &server_addr.sin_addr);

  socklen_t client_addr_len = sizeof(client_addr);
  
  while (1) {
    MovePacket move = {0};

    pthread_mutex_lock(&move_mutex);
    move.pos = *pos;
    pthread_mutex_unlock(&move_mutex);

    sendto(client_fd, (char*)&move, sizeof(move), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    MovePacket incoming_pkt = {0};

    int msg_len = recvfrom(client_fd, (char*)&incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (msg_len == 0) {
      printf("No message was able to be received\n");
    } else if (msg_len < 0) {
      perror("Failed to receive message");
      exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&move_mutex);
    client_move.pos = incoming_pkt.pos;
    pthread_mutex_unlock(&move_mutex);
  }
  
  #ifdef _WIN32
    closesocket(client_fd);
    WSACleanup();
  #else
    close(client_fd);
  #endif
  
  return NULL;
}
