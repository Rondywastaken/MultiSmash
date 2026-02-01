#include <raylib.h>
#include <pthread.h>

typedef struct MovePacket {
  Vector2 pos;
} MovePacket;

extern MovePacket server_move;
extern MovePacket client_move;
extern pthread_mutex_t move_mutex;

// function for the server thread to run
extern void *server_thread(void *arg);

// function for the server thread to run
extern void *client_thread(void *arg);


