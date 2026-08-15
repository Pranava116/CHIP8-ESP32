#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "menu.h"

AppState currentState = STATE_MENU;

const char* menuItems[] = {
    "Menu",
    "Tetris",
    "Pong",
    "IBM",
};

int menuindex = 0;
int totalitems = 4;

void drawMenu() {
    tft.fillScreen(ILI9341_BLACK);

    // Draw header title
    tft.setTextSize(3);
    tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
    tft.setCursor(50, 20);
    tft.println("CHIP-8 MENU");

    // Divider line below header
    tft.drawFastHLine(20, 55, 280, ILI9341_CYAN);

    // Draw menu options
    tft.setTextSize(2);
    for (int i = 0; i < totalitems; i++) {
        int yPos = 75 + (i * 32);
        tft.setCursor(60, yPos);
        if (i == menuindex) {
            tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE); // Highlighted item
            tft.print("> ");
            tft.print(menuItems[i]);
            tft.print(" <");
        } else {
            tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
            tft.print("  ");
            tft.print(menuItems[i]);
            tft.print("  ");
        }
    }
}








