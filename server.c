#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  Vector2 pos; 
} MovePacket;

struct sockaddr_in player1_addr = {0};
struct sockaddr_in player2_addr = {0};
int p1_connected = 0;
int p2_connected = 0;

int main(void) {
  struct sockaddr_in server_addr, incoming_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(8080);
  socklen_t server_addr_len = sizeof(server_addr);
  socklen_t incoming_addr_len = sizeof(incoming_addr);

  int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_fd < 0) {
    perror("Server socket failed to create");
    exit(EXIT_FAILURE);
  }

  if (bind(server_fd, (struct sockaddr*)&server_addr, server_addr_len) < 0) {
    perror("Failed to bind socket");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port 8080\n");

  while (1) {
    MovePacket incoming_pkt;
    int msg_len = recvfrom(server_fd, &incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&incoming_addr, &incoming_addr_len);
    if (msg_len == 0) {
      printf("No message was able to be received\n");
    } else if (msg_len < 0) {
      perror("Failed to receive message");
      exit(EXIT_FAILURE);
    }

    // make sure players are connected. This can be improved later with a better struct with more info sent from client
    if (!p1_connected) { 
      player1_addr = incoming_addr;
      p1_connected = 1;
      printf("Player 1 has joined remote server from %d:%d", player1_addr.sin_addr.s_addr, player1_addr.sin_port);
      continue;
    }
    else if (!p2_connected) {
      if (incoming_addr.sin_addr.s_addr != player1_addr.sin_addr.s_addr ||
          incoming_addr.sin_port != player1_addr.sin_port) {
        player2_addr = incoming_addr;
        p2_connected = 1;
        printf("Player 2 has joined remote server from %d:%d", player2_addr.sin_addr.s_addr, player2_addr.sin_port);
      }
      continue;
    }

    // message coming from player1 client
    if (incoming_addr.sin_addr.s_addr == player1_addr.sin_addr.s_addr &&
        incoming_addr.sin_port == player1_addr.sin_port) {
      sendto(server_fd, &incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&player2_addr, sizeof(player2_addr));
    }

    // message coming from player2 client
    if (incoming_addr.sin_addr.s_addr == player2_addr.sin_addr.s_addr &&
        incoming_addr.sin_port == player2_addr.sin_port) {
      sendto(server_fd, &incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&player1_addr, sizeof(player1_addr));
    }
  }
  return EXIT_SUCCESS;
}

