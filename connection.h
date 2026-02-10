#ifdef _WIN32
  // Prevent Windows headers from defining things that clash with raylib
  #define NOGDI             // All GDI defines and routines (clashes with Rectangle)
  #define NOUSER            // All USER defines and routines (clashes with CloseWindow)
  #define WIN32_LEAN_AND_MEAN // Speed up build and reduce header bloat

  // include the ones I need
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

extern MovePacket server_move;
extern MovePacket client_move;
extern pthread_mutex_t move_mutex;

// function for the server thread to run
extern void *server_thread(void *arg);

// function for the server thread to run
extern void *client_thread(void *arg);


