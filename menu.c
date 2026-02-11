#include "menu.h"
#include "raylib.h"
#include "state_machine.h"

Button btn_1;
Button btn_2;
Button btn_input;
Text txt_1;
Text txt_2;
int active_btn = 0;

Rectangle textbox;

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

  textbox.x = screen_w / 2.0f + 10.0f;
  textbox.y = btn_input.pos.y + (btn_input.h / 2.0f) - (40.0f / 2.0f);
  textbox.height = screen_h * 0.1f + 10.0f;
  textbox.width = screen_w * 0.5f + 10.0f;
}

void DrawMenu(void) {
  DrawRectangle(btn_1.pos.x, btn_1.pos.y, btn_1.w, btn_1.h, btn_1.color);
  DrawText("HOST GAME", txt_1.pos.x, txt_1.pos.y, txt_1.font_size, txt_1.color);
  DrawRectangle(btn_2.pos.x, btn_2.pos.y, btn_2.w, btn_2.h, btn_2.color);
  DrawText("JOIN HOST", txt_2.pos.x, txt_2.pos.y, txt_2.font_size, txt_2.color);
  DrawRectangle(btn_input.pos.x, btn_input.pos.y, btn_input.w, btn_input.h, btn_input.color);
  DrawRectangleLines(textbox.x, textbox.y, textbox.width, textbox.height, GREEN);
}

Connection UpdateMenu(void) {
  if (IsKeyPressed(KEY_UP)) {
    btn_1.color = LIME;
    btn_2.color = LIGHTGRAY;
    active_btn = 0;
  } else if (IsKeyPressed(KEY_DOWN)) {
    btn_2.color = LIME;
    btn_1.color = LIGHTGRAY;
    active_btn = 1;
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
