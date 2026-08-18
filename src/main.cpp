#ifdef ARDUINO

#include "chip8.h"
#include "menu.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <Keypad.h>
#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                         {'4', '5', '6', 'B'},
                         {'7', '8', '9', 'C'},
                         {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {27, 14, 12, 13};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

Chip8 chip8;

// Display scaling settings (dynamically calculated for 240x320 or 320x240
// screens)
int SCALE = 5;
int X_OFFSET = 0;
int Y_OFFSET = 40;

void calculateScaling() {
  int scaleX = tft.width() / Chip8::VIDEO_WIDTH;
  int scaleY = tft.height() / Chip8::VIDEO_HEIGHT;
  SCALE = (scaleX < scaleY) ? scaleX : scaleY;
  if (SCALE < 1)
    SCALE = 1;

  X_OFFSET = (tft.width() - (Chip8::VIDEO_WIDTH * SCALE)) / 2;
  Y_OFFSET = (tft.height() - (Chip8::VIDEO_HEIGHT * SCALE)) / 2;
}

uint32_t prevVideo[Chip8::VIDEO_WIDTH * Chip8::VIDEO_HEIGHT];

const uint8_t default_rom[] = {
    0xA2, 0xB4, 0x23, 0xE6, 0x22, 0xB6, 0x70, 0x01, 0xD0, 0x11, 0x30, 0x25,
    0x12, 0x06, 0x71, 0xFF, 0xD0, 0x11, 0x60, 0x1A, 0xD0, 0x11, 0x60, 0x25,
    0x31, 0x00, 0x12, 0x0E, 0xC4, 0x70, 0x44, 0x70, 0x12, 0x1C, 0xC3, 0x03,
    0x60, 0x1E, 0x61, 0x03, 0x22, 0x5C, 0xF5, 0x15, 0xD0, 0x14, 0x3F, 0x01,
    0x12, 0x3C, 0xD0, 0x14, 0x71, 0xFF, 0xD0, 0x14, 0x23, 0x40, 0x12, 0x1C,
    0xE7, 0xA1, 0x22, 0x72, 0xE8, 0xA1, 0x22, 0x84, 0xE9, 0xA1, 0x22, 0x96,
    0xE2, 0x9E, 0x12, 0x50, 0x66, 0x00, 0xF6, 0x15, 0xF6, 0x07, 0x36, 0x00,
    0x12, 0x3C, 0xD0, 0x14, 0x71, 0x01, 0x12, 0x2A, 0xA2, 0xC4, 0xF4, 0x1E,
    0x66, 0x00, 0x43, 0x01, 0x66, 0x04, 0x43, 0x02, 0x66, 0x08, 0x43, 0x03,
    0x66, 0x0C, 0xF6, 0x1E, 0x00, 0xEE, 0xD0, 0x14, 0x70, 0xFF, 0x23, 0x34,
    0x3F, 0x01, 0x00, 0xEE, 0xD0, 0x14, 0x70, 0x01, 0x23, 0x34, 0x00, 0xEE,
    0xD0, 0x14, 0x70, 0x01, 0x23, 0x34, 0x3F, 0x01, 0x00, 0xEE, 0xD0, 0x14,
    0x70, 0xFF, 0x23, 0x34, 0x00, 0xEE, 0xD0, 0x14, 0x73, 0x01, 0x43, 0x04,
    0x63, 0x00, 0x22, 0x5C, 0x23, 0x34, 0x3F, 0x01, 0x00, 0xEE, 0xD0, 0x14,
    0x73, 0xFF, 0x43, 0xFF, 0x63, 0x03, 0x22, 0x5C, 0x23, 0x34, 0x00, 0xEE,
    0x80, 0x00, 0x67, 0x05, 0x68, 0x06, 0x69, 0x04, 0x61, 0x1F, 0x65, 0x10,
    0x62, 0x07, 0x00, 0xEE, 0x40, 0xE0, 0x00, 0x00, 0x40, 0xC0, 0x40, 0x00,
    0x00, 0xE0, 0x40, 0x00, 0x40, 0x60, 0x40, 0x00, 0x40, 0x40, 0x60, 0x00,
    0x20, 0xE0, 0x00, 0x00, 0xC0, 0x40, 0x40, 0x00, 0x00, 0xE0, 0x80, 0x00,
    0x40, 0x40, 0xC0, 0x00, 0x00, 0xE0, 0x20, 0x00, 0x60, 0x40, 0x40, 0x00,
    0x80, 0xE0, 0x00, 0x00, 0x40, 0xC0, 0x80, 0x00, 0xC0, 0x60, 0x00, 0x00,
    0x40, 0xC0, 0x80, 0x00, 0xC0, 0x60, 0x00, 0x00, 0x80, 0xC0, 0x40, 0x00,
    0x00, 0x60, 0xC0, 0x00, 0x80, 0xC0, 0x40, 0x00, 0x00, 0x60, 0xC0, 0x00,
    0xC0, 0xC0, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00,
    0xC0, 0xC0, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x00, 0xF0, 0x00, 0x00,
    0x40, 0x40, 0x40, 0x40, 0x00, 0xF0, 0x00, 0x00, 0xD0, 0x14, 0x66, 0x35,
    0x76, 0xFF, 0x36, 0x00, 0x13, 0x38, 0x00, 0xEE, 0xA2, 0xB4, 0x8C, 0x10,
    0x3C, 0x1E, 0x7C, 0x01, 0x3C, 0x1E, 0x7C, 0x01, 0x3C, 0x1E, 0x7C, 0x01,
    0x23, 0x5E, 0x4B, 0x0A, 0x23, 0x72, 0x91, 0xC0, 0x00, 0xEE, 0x71, 0x01,
    0x13, 0x50, 0x60, 0x1B, 0x6B, 0x00, 0xD0, 0x11, 0x3F, 0x00, 0x7B, 0x01,
    0xD0, 0x11, 0x70, 0x01, 0x30, 0x25, 0x13, 0x62, 0x00, 0xEE, 0x60, 0x1B,
    0xD0, 0x11, 0x70, 0x01, 0x30, 0x25, 0x13, 0x74, 0x8E, 0x10, 0x8D, 0xE0,
    0x7E, 0xFF, 0x60, 0x1B, 0x6B, 0x00, 0xD0, 0xE1, 0x3F, 0x00, 0x13, 0x90,
    0xD0, 0xE1, 0x13, 0x94, 0xD0, 0xD1, 0x7B, 0x01, 0x70, 0x01, 0x30, 0x25,
    0x13, 0x86, 0x4B, 0x00, 0x13, 0xA6, 0x7D, 0xFF, 0x7E, 0xFF, 0x3D, 0x01,
    0x13, 0x82, 0x23, 0xC0, 0x3F, 0x01, 0x23, 0xC0, 0x7A, 0x01, 0x23, 0xC0,
    0x80, 0xA0, 0x6D, 0x07, 0x80, 0xD2, 0x40, 0x04, 0x75, 0xFE, 0x45, 0x02,
    0x65, 0x04, 0x00, 0xEE, 0xA7, 0x00, 0xF2, 0x55, 0xA8, 0x04, 0xFA, 0x33,
    0xF2, 0x65, 0xF0, 0x29, 0x6D, 0x32, 0x6E, 0x00, 0xDD, 0xE5, 0x7D, 0x05,
    0xF1, 0x29, 0xDD, 0xE5, 0x7D, 0x05, 0xF2, 0x29, 0xDD, 0xE5, 0xA7, 0x00,
    0xF2, 0x65, 0xA2, 0xB4, 0x00, 0xEE, 0x6A, 0x00, 0x60, 0x19, 0x00, 0xEE,
    0x37, 0x23};

uint8_t mapKeyToHex(char k) {
  switch (k) {
  case '1':
    return 0x1;
  case '2':
    return 0x2;
  case '3':
    return 0x3;
  case 'A':
    return 0xC;

  case '4':
    return 0x4;
  case '5':
    return 0x5;
  case '6':
    return 0x6;
  case 'B':
    return 0xD;

  case '7':
    return 0x7;
  case '8':
    return 0x8;
  case '9':
    return 0x9;
  case 'C':
    return 0xE;

  case '*':
    return 0xA;
  case '0':
    return 0x0;
  case '#':
    return 0xB;
  case 'D':
    return 0xF;
  default:
    return 0xFF;
  }
}

void updateKeypad() {
  memset(chip8.keypad, 0, sizeof(chip8.keypad));
  if (keypad.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) {
      if (keypad.key[i].stateChanged) {
      }
      if (keypad.key[i].kstate == PRESSED || keypad.key[i].kstate == HOLD) {
        uint8_t hexKey = mapKeyToHex(keypad.key[i].kchar);
        if (hexKey < 16) {
          chip8.keypad[hexKey] = 1;
        }
      }
    }
  }
}

void renderDisplay() {
  for (size_t i = 0; i < Chip8::VIDEO_WIDTH * Chip8::VIDEO_HEIGHT; ++i) {
    if (chip8.video[i] != prevVideo[i]) {
      int col = i % Chip8::VIDEO_WIDTH;
      int row = i / Chip8::VIDEO_WIDTH;
      uint16_t color = (chip8.video[i] != 0) ? ILI9341_WHITE : ILI9341_BLACK;

      tft.fillRect(X_OFFSET + col * SCALE, Y_OFFSET + row * SCALE, SCALE, SCALE,
                   color);
      prevVideo[i] = chip8.video[i];
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing CHIP-8 Emulator on ESP32...");

  tft.begin();
  tft.setRotation(1);
  calculateScaling();
  tft.fillScreen(ILI9341_BLACK);

  memset(prevVideo, 0, sizeof(prevVideo));

  if (chip8.LoadROMFromBuffer(default_rom, sizeof(default_rom))) {
    Serial.println("Tetris ROM loaded successfully!");
  } else {
    Serial.println("Failed to load ROM!");
  }

  if (currentState == STATE_MENU) {
    drawMenu();
  }
}

unsigned long lastFrameTime = 0;
const int cyclesPerFrame = 10;
const int frameDelayMs = 16;

void loop() {
  if (currentState == STATE_MENU) {
    updateMenu();
  } else if (currentState == STATE_RUNNING) {
    unsigned long currentTime = millis();

    if (currentTime - lastFrameTime >= frameDelayMs) {
      lastFrameTime = currentTime;

      updateKeypad();

      for (int i = 0; i < cyclesPerFrame; ++i) {
        chip8.Cycle();
      }

      renderDisplay();
    }
  }
  delay(1);
}

#else // Native Desktop / SDL2 Mode

#define SDL_MAIN_HANDLED
#include "chip8.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include <string>

const int SCALE = 16;
const int SCREEN_WIDTH = Chip8::VIDEO_WIDTH * SCALE;
const int SCREEN_HEIGHT = Chip8::VIDEO_HEIGHT * SCALE;

void ProcessInput(uint8_t *keypad, bool &quit) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
    } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
      uint8_t state = (e.type == SDL_KEYDOWN) ? 1 : 0;
      switch (e.key.keysym.sym) {
      case SDLK_ESCAPE:
        quit = true;
        break;
      case SDLK_1:
        keypad[0x1] = state;
        break;
      case SDLK_2:
        keypad[0x2] = state;
        break;
      case SDLK_3:
        keypad[0x3] = state;
        break;
      case SDLK_4:
        keypad[0xC] = state;
        break;

      case SDLK_q:
        keypad[0x4] = state;
        break;
      case SDLK_w:
        keypad[0x5] = state;
        break;
      case SDLK_e:
        keypad[0x6] = state;
        break;
      case SDLK_r:
        keypad[0xD] = state;
        break;

      case SDLK_a:
        keypad[0x7] = state;
        break;
      case SDLK_s:
        keypad[0x8] = state;
        break;
      case SDLK_d:
        keypad[0x9] = state;
        break;
      case SDLK_f:
        keypad[0xE] = state;
        break;

      case SDLK_z:
        keypad[0xA] = state;
        break;
      case SDLK_x:
        keypad[0x0] = state;
        break;
      case SDLK_c:
        keypad[0xB] = state;
        break;
      case SDLK_v:
        keypad[0xF] = state;
        break;
      default:
        break;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  std::string romPath;
  if (argc > 1) {
    romPath = argv[1];
  } else {
    std::cout << "Enter path to CHIP-8 ROM file (default: roms/ibm_logo.ch8): ";
    std::getline(std::cin, romPath);
    if (romPath.empty()) {
      romPath = "roms/ibm_logo.ch8";
    }
  }

  if (romPath.length() >= 2 && romPath.front() == '"' &&
      romPath.back() == '"') {
    romPath = romPath.substr(1, romPath.length() - 2);
  }

  Chip8 chip8;
  if (!chip8.LoadROM(romPath.c_str())) {
    std::cerr << "Failed to open ROM file: " << romPath << std::endl;
    return 1;
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

  SDL_SetMainReady();
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "CHIP-8 Emulator (SDL2)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << "Failed to create SDL Window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {
    std::cerr << "Failed to create SDL Renderer: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_RenderSetLogicalSize(renderer, Chip8::VIDEO_WIDTH, Chip8::VIDEO_HEIGHT);

  SDL_Texture *texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      Chip8::VIDEO_WIDTH, Chip8::VIDEO_HEIGHT);

  if (!texture) {
    std::cerr << "Failed to create SDL Texture: " << SDL_GetError()
              << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  uint32_t pixels[Chip8::VIDEO_WIDTH * Chip8::VIDEO_HEIGHT];
  bool quit = false;
  int cycleDelay = 2;
  int cyclesPerFrame = 10;

  auto lastCycleTime = std::chrono::high_resolution_clock::now();

  std::cout << "Starting CHIP-8 Emulator..." << std::endl;
  std::cout << "Controls (QWERTY hex keypad):" << std::endl;
  std::cout << "  1 2 3 4 -> 1 2 3 C" << std::endl;
  std::cout << "  Q W E R -> 4 5 6 D" << std::endl;
  std::cout << "  A S D F -> 7 8 9 E" << std::endl;
  std::cout << "  Z X C V -> A 0 B F" << std::endl;
  std::cout << "  ESC     -> Quit" << std::endl;

  while (!quit) {
    ProcessInput(chip8.keypad, quit);

    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(
                   currentTime - lastCycleTime)
                   .count();

    if (dt >= cycleDelay) {
      lastCycleTime = currentTime;

      for (int i = 0; i < cyclesPerFrame; ++i) {
        chip8.Cycle();
      }

      for (size_t i = 0; i < Chip8::VIDEO_WIDTH * Chip8::VIDEO_HEIGHT; ++i) {
        uint32_t pixel = chip8.video[i];
        pixels[i] = (pixel != 0) ? 0x00FF66FF : 0x121420FF;
      }

      SDL_UpdateTexture(texture, nullptr, pixels,
                        Chip8::VIDEO_WIDTH * sizeof(uint32_t));
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, nullptr, nullptr);
      SDL_RenderPresent(renderer);
    }

    SDL_Delay(1);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

#endif
