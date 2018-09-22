#include <string.h>

extern "C" {
#include "cassl1.h"
}

void register_mazoyer_states(void)
{
  state state;

  strcpy(state.name, "WWW");
  state.fg_rgb_color[2] = 0x00;
  state.fg_rgb_color[1] = 0x00;
  state.fg_rgb_color[0] = 0x00;
  state.bg_rgb_color[2] = 0xff;
  state.bg_rgb_color[1] = 0xff;
  state.bg_rgb_color[0] = 0xff;
  state.type = EXTERNAL;
  register_state(state);

  strcpy(state.name, "L");
  state.fg_rgb_color[2] = 0x00;
  state.fg_rgb_color[1] = 0x00;
  state.fg_rgb_color[0] = 0x00;
  state.bg_rgb_color[2] = 0xFF;
  state.bg_rgb_color[1] = 0xFF;
  state.bg_rgb_color[0] = 0xFF;
  state.type = SOLDIER;
  register_state(state);

  strcpy(state.name, "A");
  state.fg_rgb_color[2] = 0x00;
  state.fg_rgb_color[1] = 0x00;
  state.fg_rgb_color[0] = 0x00;
  state.bg_rgb_color[2] = 0xFF;
  state.bg_rgb_color[1] = 0x80;
  state.bg_rgb_color[0] = 0x80;
  state.type = OTHERS;
  register_state(state);

  strcpy(state.name, "B");
  state.fg_rgb_color[2] = 0x00;
  state.fg_rgb_color[1] = 0x00;
  state.fg_rgb_color[0] = 0x00;
  state.bg_rgb_color[2] = 0x00;
  state.bg_rgb_color[1] = 0xFF;
  state.bg_rgb_color[0] = 0x00;
  state.type = OTHERS;
  register_state(state);

  strcpy(state.name, "C");
  state.fg_rgb_color[2] = 0x00;
  state.fg_rgb_color[1] = 0x00;
  state.fg_rgb_color[0] = 0x00;
  state.bg_rgb_color[2] = 0x80;
  state.bg_rgb_color[1] = 0x00;
  state.bg_rgb_color[0] = 0xFF;
  state.type = OTHERS;
  register_state(state);

  strcpy(state.name, "G");
  state.fg_rgb_color[2] = 0x00;
  state.fg_rgb_color[1] = 0x00;
  state.fg_rgb_color[0] = 0x00;
  state.bg_rgb_color[2] = 0x00;
  state.bg_rgb_color[1] = 0xFF;
  state.bg_rgb_color[0] = 0xFF;
  state.type = GENERAL;
  register_state(state);

  strcpy(state.name, "F");
  state.fg_rgb_color[2] = 0x00;
  state.fg_rgb_color[1] = 0x00;
  state.fg_rgb_color[0] = 0x00;
  state.bg_rgb_color[2] = 0x00;
  state.bg_rgb_color[1] = 0x88;
  state.bg_rgb_color[0] = 0xFF;
  state.type = FIRING;
  register_state(state);

}

void register_mazoyer_rules(void)
{
  register_rule("WWW","L","L","L");
  register_rule("L","L","WWW","L");
  register_rule("L","L","L","L");
  register_rule("L","L","B","L");
  register_rule("L","L","C","L");
  register_rule("L","L","G","L");
  register_rule("A","L","WWW","C");
  register_rule("A","L","L","G");
  register_rule("A","L","A","L");
  register_rule("A","L","B","L");
  register_rule("A","L","C","L");
  register_rule("A","L","G","C");
  register_rule("B","L","WWW","L");
  register_rule("B","L","L","L");
  register_rule("B","L","A","L");
  register_rule("B","L","B","L");
  register_rule("B","L","C","L");
  register_rule("B","L","G","L");
  register_rule("C","L","WWW","G");
  register_rule("C","L","L","A");
  register_rule("C","L","A","L");
  register_rule("C","L","B","L");
  register_rule("C","L","C","L");
  register_rule("C","L","G","G");
  register_rule("G","L","WWW","A");
  register_rule("G","L","L","C");
  register_rule("G","L","A","L");
  register_rule("G","L","B","L");
  register_rule("G","L","C","L");
  register_rule("G","L","G","A");
  register_rule("WWW","A","A","F");
  register_rule("WWW","A","C","G");
  register_rule("L","A","A","A");
  register_rule("L","A","B","L");
  register_rule("L","A","C","G");
  register_rule("A","A","WWW","F");
  register_rule("A","A","L","A");
  register_rule("A","A","A","A");
  register_rule("A","A","B","B");
  register_rule("A","A","C","C");
  register_rule("A","A","G","B");
  register_rule("B","A","WWW","C");
  register_rule("B","A","L","G");
  register_rule("B","A","B","G");
  register_rule("B","A","C","C");
  register_rule("B","A","G","C");
  register_rule("C","A","L","A");
  register_rule("C","A","A","A");
  register_rule("G","A","WWW","C");
  register_rule("G","A","C","C");
  register_rule("G","A","G","C");
  register_rule("L","B","A","G");
  register_rule("L","B","B","B");
  register_rule("L","B","C","L");
  register_rule("L","B","G","B");
  register_rule("A","B","L","G");
  register_rule("A","B","A","B");
  register_rule("A","B","B","B");
  register_rule("A","B","C","L");
  register_rule("B","B","L","G");
  register_rule("B","B","A","A");
  register_rule("B","B","B","B");
  register_rule("B","B","C","C");
  register_rule("B","B","G","B");
  register_rule("C","B","WWW","L");
  register_rule("C","B","L","L");
  register_rule("C","B","A","A");
  register_rule("C","B","G","L");
  register_rule("G","B","WWW","G");
  register_rule("G","B","L","C");
  register_rule("G","B","A","C");
  register_rule("G","B","C","B");
  register_rule("G","B","G","G");
  register_rule("L","C","L","C");
  register_rule("L","C","A","A");
  register_rule("L","C","B","G");
  register_rule("L","C","C","C");
  register_rule("L","C","G","G");
  register_rule("A","C","WWW","B");
  register_rule("A","C","L","B");
  register_rule("A","C","B","B");
  register_rule("A","C","G","B");
  register_rule("B","C","WWW","G");
  register_rule("B","C","L","C");
  register_rule("B","C","C","C");
  register_rule("B","C","G","G");
  register_rule("C","C","L","C");
  register_rule("C","C","A","A");
  register_rule("C","C","B","B");
  register_rule("C","C","C","C");
  register_rule("C","C","G","B");
  register_rule("G","C","WWW","B");
  register_rule("G","C","L","B");
  register_rule("G","C","B","B");
  register_rule("G","C","G","B");
  register_rule("WWW","G","L","A");
  register_rule("WWW","G","B","G");
  register_rule("WWW","G","C","G");
  register_rule("WWW","G","G","F");
  register_rule("L","G","A","G");
  register_rule("L","G","B","G");
  register_rule("L","G","C","G");
  register_rule("A","G","L","B");
  register_rule("A","G","B","G");
  register_rule("A","G","C","G");
  register_rule("B","G","WWW","G");
  register_rule("B","G","L","B");
  register_rule("B","G","B","G");
  register_rule("B","G","C","G");
  register_rule("B","G","G","G");
  register_rule("C","G","WWW","A");
  register_rule("C","G","L","A");
  register_rule("C","G","B","G");
  register_rule("C","G","C","G");
  register_rule("C","G","G","A");
  register_rule("G","G","WWW","F");
  register_rule("G","G","L","B");
  register_rule("G","G","B","G");
  register_rule("G","G","C","G");
  register_rule("G","G","G","F");
}
