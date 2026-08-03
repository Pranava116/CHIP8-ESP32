#ifdef ARDUINO
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <chip8.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
    tft.init();
    tft.setRotation(1);
    //other init functions like intializing the cycle and also upscaling the chip8 resolution to match the tft resolution
}               

void loop() {
    
    //take input here mate 
        // auto currentTime = std::chrono::high_resolution_clock::now();
        // float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        
        // if (dt >= cycleDelay) {
        //     lastCycleTime = currentTime;

        //     for (int i = 0; i < cyclesPerFrame; ++i) {
        //         chip8.Cycle();
        //     }

        //     for (size_t i = 0; i < Chip8::VIDEO_WIDTH * Chip8::VIDEO_HEIGHT; ++i) {
        //         uint32_t pixel = chip8.video[i];
        //         pixels[i] = (pixel != 0) ? 0x00FF66FF : 0x121420FF;
        //     }

        //     SDL_UpdateTexture(texture, nullptr, pixels, Chip8::VIDEO_WIDTH * sizeof(uint32_t));
        //     SDL_RenderClear(renderer);
        //     SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        //     SDL_RenderPresent(renderer);
        // }
}

#else // Native Desktop / SDL2 Mode

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <chip8.h>
#include <iostream>
#include <chrono>
#include <string>

const int SCALE = 16;
const int SCREEN_WIDTH = Chip8::VIDEO_WIDTH * SCALE;
const int SCREEN_HEIGHT = Chip8::VIDEO_HEIGHT * SCALE;

void ProcessInput(uint8_t* keypad, bool& quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            uint8_t state = (e.type == SDL_KEYDOWN) ? 1 : 0;
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE: quit = true; break;
                case SDLK_1: keypad[0x1] = state; break;
                case SDLK_2: keypad[0x2] = state; break;
                case SDLK_3: keypad[0x3] = state; break;
                case SDLK_4: keypad[0xC] = state; break;

                case SDLK_q: keypad[0x4] = state; break;
                case SDLK_w: keypad[0x5] = state; break;
                case SDLK_e: keypad[0x6] = state; break;
                case SDLK_r: keypad[0xD] = state; break;

                case SDLK_a: keypad[0x7] = state; break;
                case SDLK_s: keypad[0x8] = state; break;
                case SDLK_d: keypad[0x9] = state; break;
                case SDLK_f: keypad[0xE] = state; break;

                case SDLK_z: keypad[0xA] = state; break;
                case SDLK_x: keypad[0x0] = state; break;
                case SDLK_c: keypad[0xB] = state; break;
                case SDLK_v: keypad[0xF] = state; break;
                default: break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::string romPath;
    if (argc > 1) {
        romPath = argv[1];
    } else {
        std::cout << "Enter path to CHIP-8 ROM file: ";
        std::getline(std::cin, romPath);
        if (romPath.empty()) {
            std::cout << "No ROM specified. Exiting..." << std::endl;
            return 1;
        }
    }

    if (romPath.length() >= 2 && romPath.front() == '"' && romPath.back() == '"') {
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

    SDL_Window* window = SDL_CreateWindow(
        "CHIP-8 Emulator (SDL2)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Failed to create SDL Window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "Failed to create SDL Renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, Chip8::VIDEO_WIDTH, Chip8::VIDEO_HEIGHT);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        Chip8::VIDEO_WIDTH,
        Chip8::VIDEO_HEIGHT
    );

    if (!texture) {
        std::cerr << "Failed to create SDL Texture: " << SDL_GetError() << std::endl;
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
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt >= cycleDelay) {
            lastCycleTime = currentTime;

            for (int i = 0; i < cyclesPerFrame; ++i) {
                chip8.Cycle();
            }

            for (size_t i = 0; i < Chip8::VIDEO_WIDTH * Chip8::VIDEO_HEIGHT; ++i) {
                uint32_t pixel = chip8.video[i];
                pixels[i] = (pixel != 0) ? 0x00FF66FF : 0x121420FF;
            }

            SDL_UpdateTexture(texture, nullptr, pixels, Chip8::VIDEO_WIDTH * sizeof(uint32_t));
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
