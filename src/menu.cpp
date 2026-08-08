#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

enum AppState{
    MENU,
    RUNNING,
    PAUSED,
};

AppState currentState = MENU;

const char* menuItems[] = {
    "Menu",
    "Tetris",
    "Pong",
    "IBM",
};

int menuindex = 0;
int totalitems = 4;

void drawMenu(){
    for(int i =0;i<totalitems;i++){
        if(i == menuindex){
             tft.setTextColor(TFT_BLACK, TFT_WHITE); // highlighted
        }
        else{
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
        }
    }
}



