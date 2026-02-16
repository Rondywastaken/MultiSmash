#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <pthread.h>

#include "packet.h"
#include "connection.h"
#include "menu.h"

Packet host_move = {0};
Packet guest_move = {0};
Packet remote_move = {0};

pthread_mutex_t move_mutex = PTHREAD_MUTEX_INITIALIZER;

void *host_thread(void* arg) {
  Packet *pkt = (Packet*)arg;

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
    Packet incoming_pkt = {0};

    // Receive messages
    int msg_len = recvfrom(server_fd, (char*)&incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (msg_len == 0) {
      printf("No message was able to be received\n");
    } else if (msg_len < 0) {
      perror("Failed to receive message");
      exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&move_mutex);
    host_move.pos = incoming_pkt.pos;
    pthread_mutex_unlock(&move_mutex);

    Packet outgoing_pkt = {0};

    pthread_mutex_lock(&move_mutex);
    outgoing_pkt.pkt_type = pkt->pkt_type;
    outgoing_pkt.pos.x = pkt->pos.x;
    outgoing_pkt.pos.y = pkt->pos.y;
    pthread_mutex_unlock(&move_mutex);

    sendto(server_fd, (char*)&outgoing_pkt, sizeof(outgoing_pkt), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
  }

  #ifdef _WIN32
    closesocket(server_fd);
    WSACleanup();
  #else
    close(server_fd);
  #endif

  return NULL;
}

void *guest_thread(void *arg) {
  Packet *pkt = (Packet*)arg;

  int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in server_addr, client_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);

  const char *host_ip = GetIP();

  inet_pton(AF_INET, host_ip, &server_addr.sin_addr);

  socklen_t client_addr_len = sizeof(client_addr);

  double last_print_time = GetTime();
  int packets_received_counter = 0;

  while (1) {
    Packet outgoing_pkt = {0};

    pthread_mutex_lock(&move_mutex);
    outgoing_pkt.pkt_type = pkt->pkt_type;
    outgoing_pkt.pos.x = pkt->pos.x;
    outgoing_pkt.pos.y = pkt->pos.y;
    pthread_mutex_unlock(&move_mutex);

    sendto(client_fd, (char*)&outgoing_pkt, sizeof(outgoing_pkt), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    Packet incoming_pkt = {0};

    int msg_len = recvfrom(client_fd, (char*)&incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (msg_len == 0) {
      printf("No message was able to be received\n");
    } else if (msg_len < 0) {
      perror("Failed to receive message");
      exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&move_mutex);
    guest_move.pos = incoming_pkt.pos;
    pthread_mutex_unlock(&move_mutex);
    packets_received_counter++;

    double current_time = GetTime();
    if (current_time - last_print_time >= 1.0) {
      printf("Status: Connected | Packets RX/sec: %d\n", packets_received_counter);
      
      // Reset counter and timer
      packets_received_counter = 0;
      last_print_time = current_time;
    }
  }

  #ifdef _WIN32
    closesocket(client_fd);
    WSACleanup();
  #else
    close(client_fd);
  #endif

  return NULL;
}

void *client_thread(void *arg) {
  Packet *pkt = (Packet*)arg;

  int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in server_addr, client_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  const char *host_ip = "79.76.34.160";
  inet_pton(AF_INET, host_ip, &server_addr.sin_addr);
  socklen_t client_addr_len = sizeof(client_addr);

  double last_print_time = GetTime();
  int packets_received_counter = 0;

  while (pkt->pkt_type != PKT_DISCONNECT) {
    pthread_mutex_lock(&move_mutex);
    Packet outgoing_pkt = *pkt;
    pthread_mutex_unlock(&move_mutex);
    sendto(client_fd, (char*)&outgoing_pkt, sizeof(outgoing_pkt), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(client_fd, &readfds);
    struct timeval tv = {0, 0};

    // Peak into client socket, so we don't end up deadlocking the program with recvfrom
    int ready = select(client_fd+1, &readfds, NULL, NULL, &tv);
    if (ready > 0) {
      Packet incoming_pkt = {0};
      int msg_len = recvfrom(client_fd, (char*)&incoming_pkt, sizeof(incoming_pkt), 0, (struct sockaddr*)&client_addr, &client_addr_len);
      if (msg_len > 0) {
        pthread_mutex_lock(&move_mutex);
        remote_move.pos = incoming_pkt.pos;
        pthread_mutex_unlock(&move_mutex);
        packets_received_counter++;
      }
    }

    double current_time = GetTime();
    if (current_time - last_print_time >= 1.0) {
      printf("Status: Connected | Packets RX/sec: %d\n", packets_received_counter);
      
      // Reset counter and timer
      packets_received_counter = 0;
      last_print_time = current_time;
    }
  }

  Packet disconnect_pkt = {0};
  disconnect_pkt.pkt_type = PKT_DISCONNECT;
  sendto(client_fd, (char*)&disconnect_pkt, sizeof(disconnect_pkt), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

  #ifdef _WIN32
    closesocket(client_fd);
    WSACleanup();
  #else
    close(client_fd);
  #endif

  return NULL;
}
