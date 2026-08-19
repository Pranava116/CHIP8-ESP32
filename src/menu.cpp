#include "menu.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <Keypad.h>

AppState currentState = STATE_MENU;

const char *menuItems[] = {
    "Menu",
    "Tetris",
    "Pong",
};

int menuindex = 0;
int totalitems = sizeof(menuItems) / sizeof(menuItems[0]);

void drawMenuItems() {
  tft.setTextSize(2);
  for (int i = 0; i < totalitems; i++) {
    int yPos = 75 + (i * 32);
    if (i == menuindex) {
      // Highlighted item
      tft.fillRect(40, yPos - 4, 240, 24, ILI9341_WHITE);
      tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
      tft.setCursor(60, yPos);
      tft.print("> ");
      tft.print(menuItems[i]);
      tft.print(" <");
    } else {
      // Unselected item
      tft.fillRect(40, yPos - 4, 240, 24, ILI9341_BLACK);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.setCursor(60, yPos);
      tft.print("  ");
      tft.print(menuItems[i]);
      tft.print("  ");
    }
  }
}

void drawMenu() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(50, 20);
  tft.println("CHIP-8 MENU");
  tft.drawFastHLine(20, 55, 280, ILI9341_CYAN);
  drawMenuItems();
}

char updateMenu() {
  char key = keypad.getKey();
  if (key) {
    int prevIndex = menuindex;
    if (key == '2' || key == 'A') {
      menuindex--;
      if (menuindex < 0) {
        menuindex = totalitems - 1;
      }
    } else if (key == '8' || key == 'B') {
      menuindex++;
      if (menuindex >= totalitems) {
        menuindex = 0;
      }
    } else if (key == '#' || key == 'D' || key == '5' || key == '*') {
      Serial.print("Selected option: ");
      Serial.println(menuItems[menuindex]);
      currentState = STATE_RUNNING;
      tft.fillScreen(ILI9341_BLACK);
      return menuItems[menuindex];
    }

    if (menuindex != prevIndex) {
      drawMenuItems();
    }
  }
}

