#ifndef PACKET_H
#define PACKET_H

typedef struct {
  float x;
  float y;
} NetVector2;

typedef enum {
  PKT_CONNECT,
  PKT_DISCONNECT,
  PKT_GAME_UPDATE,
} PacketType; 

typedef struct {
  PacketType pkt_type;
  NetVector2 pos;
} Packet; 

#endif 
