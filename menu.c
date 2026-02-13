#include "menu.h"
#include "raylib.h"
#include "state_machine.h"
#include <stdio.h>
#include <string.h>

Button btn_1;
Button btn_2;
Button btn_input;
Text txt_1;
Text txt_2;
int active_btn = 0;

Rectangle textbox;

int frames_counter = 0;

char ip[16] = "\0";

void InitMenu(int screen_w, int screen_h) {
  // Define buttons
  btn_1.w = screen_w * 0.5f;
  btn_1.h = screen_h * 0.1f;
  btn_1.pos.x = screen_w / 2.0f - btn_1.w / 2.0f;
  btn_1.pos.y = screen_h / 2.0f - 0.25f * screen_h;
  btn_1.color = LIME;
  txt_1.font_size = 40;
  txt_1.pos.x = screen_w / 2.0f - MeasureText("HOST GAME", txt_1.font_size) / 2.0f;
  txt_1.pos.y = btn_1.pos.y + (btn_1.h / 2.0f) - (txt_1.font_size / 2.0f);
  txt_1.color = MAROON;

  btn_2.w = screen_w * 0.5f;
  btn_2.h = screen_h * 0.1f;
  btn_2.pos.x = screen_w / 2.0f - btn_2.w / 2.0f;
  btn_2.pos.y = screen_h / 2.0f - 0.10f * screen_h;
  btn_2.color = LIGHTGRAY;
  txt_2.font_size = 40;
  txt_2.pos.x = screen_w / 2.0f - MeasureText("JOIN HOST", txt_2.font_size) / 2.0f;
  txt_2.pos.y = btn_2.pos.y + (btn_2.h / 2.0f) - (txt_2.font_size / 2.0f);
  txt_2.color = MAROON;

  btn_input.w = screen_w * 0.5f;
  btn_input.h = screen_h * 0.1f;
  btn_input.pos.x = screen_w / 2.0f - btn_1.w / 2.0f;
  btn_input.pos.y = screen_h / 2.0f ;
  btn_input.color = LIGHTGRAY;

  textbox.x = btn_input.pos.x;
  textbox.y = btn_input.pos.y;
  textbox.height = btn_input.h; 
  textbox.width = btn_input.w;
}

void DrawMenu(void) {
  DrawRectangle(btn_1.pos.x, btn_1.pos.y, btn_1.w, btn_1.h, btn_1.color);
  DrawText("HOST GAME", txt_1.pos.x, txt_1.pos.y, txt_1.font_size, txt_1.color);
  DrawRectangle(btn_2.pos.x, btn_2.pos.y, btn_2.w, btn_2.h, btn_2.color);
  DrawText("JOIN HOST", txt_2.pos.x, txt_2.pos.y, txt_2.font_size, txt_2.color);
  DrawRectangle(btn_input.pos.x, btn_input.pos.y, btn_input.w, btn_input.h, btn_input.color);
  DrawRectangleLinesEx(textbox, 5, MAROON);

  DrawText(ip, textbox.x + 16, textbox.y + 12, 80, MAROON);
  if (active_btn == 2)
    if (((frames_counter/20)%2) == 0) DrawText("|", textbox.x + 16 + MeasureText(ip, 80), textbox.y + 12, 80, MAROON);
}

void UpdateInputBox(int key) {
  size_t ip_len = strlen(ip);
  if (key >= 46 && key <= 58 && ip_len < 15) {
    ip[ip_len] = (char)key;
    ip[ip_len + 1] = '\0';
  }

  if (IsKeyPressed(KEY_BACKSPACE)) {
    ip[ip_len-1] = '\0';
  }
}

Connection UpdateMenu(void) {
  if (IsKeyPressed(KEY_UP) && active_btn > 0) {
    active_btn--;
  } else if (IsKeyPressed(KEY_DOWN) && active_btn < 2) {
    active_btn++;
  }

  switch (active_btn) {
    case 0:
      btn_1.color = LIME;
      btn_2.color = LIGHTGRAY;
      btn_input.color = LIGHTGRAY;
      break;
    case 1:
      btn_1.color = LIGHTGRAY;
      btn_2.color = LIME;
      btn_input.color = LIGHTGRAY;
      break;
    case 2:
      btn_1.color = LIGHTGRAY;
      btn_2.color = LIGHTGRAY;
      btn_input.color = LIME;
      UpdateInputBox(GetCharPressed()); 
      frames_counter++;
      break;
  }

  if (IsKeyPressed(KEY_ENTER)) {
    if (active_btn == 0) {
      game_state = STATE_PLAYING;
      return CONN_SERVER;
    } else if (active_btn == 1) {
      game_state = STATE_PLAYING;
      return CONN_CLIENT;
    }
  }
  return CONN_NONE;
}

char *GetIP(void) {
  return ip;
} 
