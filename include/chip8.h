#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <fstream>
#include <random>
#include <chrono>
#include <cstdlib>
#include <cstring>

class Chip8 {
public:
    uint8_t registers[16]{0};
    uint8_t memory[4096]{0};
    uint16_t pc{};
    uint8_t sp{};
    uint16_t stack[16]{0};
    uint32_t video[64 * 32]{0};
    uint16_t opcode{};
    uint16_t index{};
    uint8_t soundTimer{};
    uint8_t delayTimer{};
    uint8_t keypad[16]{0};

    static const unsigned int START_ADDR = 0x200;
    static const unsigned int FONT_SET_START_ADDR = 0x50;
    static const unsigned int VIDEO_WIDTH = 64;
    static const unsigned int VIDEO_HEIGHT = 32;

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    Chip8();
    bool LoadROM(const char* filename);
    bool LoadROMFromBuffer(const uint8_t* buffer, size_t size);

    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OP_NULL();

    void OP_00E0();
    void OP_00EE();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
    void Cycle();

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];
};

#endif // CHIP8_H
