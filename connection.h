#ifndef CONNECTION_H
#define CONNECTION_H

#ifdef _WIN32
  #define NOGDI
  #define NOUSER
  #define WIN32_LEAN_AND_MEAN
  #include <winsock2.h>
  #include <Ws2tcpip.h>
#else 
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

#include "raylib.h"
#include <pthread.h>

typedef struct {
  Vector2 pos;
} MovePacket;

typedef enum {
  CONN_NONE,
  CONN_HOST,
  CONN_GUEST,
  CONN_REMOTE
} Connection;

extern MovePacket host_move;
extern MovePacket guest_move;
extern MovePacket remote_move;
extern pthread_mutex_t move_mutex;

// function for the server thread to run
extern void *host_thread(void *arg);

// function for the server thread to run
extern void *guest_thread(void *arg);

extern void *client_thread(void *arg);

#endif
