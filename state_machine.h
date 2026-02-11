#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
  STATE_MENU,
  STATE_PLAYING,
  STATE_EXIT
} GameState;

extern GameState game_state;

#endif
