#pragma once
#include "DisplayBoard.h"
#include "pio.h"

struct Keypad {
  Pin rows[4];   // y1 = row1  , y4 = row4
  Pin column[3]; // x1 = column1 , x3 = column3
} typedef Keypad;

int press_key();
void Keypad_init();