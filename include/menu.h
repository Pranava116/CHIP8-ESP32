#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

enum AppState {
    STATE_MENU,
    STATE_RUNNING,
    STATE_PAUSED
};

extern AppState currentState;
extern Adafruit_ILI9341 tft;

void drawMenu();

#endif