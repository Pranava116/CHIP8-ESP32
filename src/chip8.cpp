#include <cstdint>
#include <fstream>
#include <random>
#include <chrono>
#include <stdlib.h>
#include <cstring>
using namespace std;

void main(){}
const unsigned int FONTSET_SIZE = 80;
   uint8_t registers[16]{0};
        uint8_t memory[4096]{0};
        uint8_t pc{};
        uint8_t sp{};
        uint8_t stack[16]{};
        uint32_t video[64 * 32]{};
        const unsigned int START_ADDR = 0x200;
        const unsigned int FONT_SET_START_ADDR = 0x50;
        uint16_t opcode;
        uint16_t index{};


uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
class Chip8{
    public:
            default_random_engine randGen;
        uniform_int_distribution<uint8_t> randByte;
        void LoadROM(const char* filename);
        Chip8();
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
};

void Chip8::LoadROM(char const* filename){
    ifstream ROM_file;
    ROM_file.open(filename,ios :: binary | ios::ate);
    if(ROM_file.is_open()){
        streampos size = ROM_file.tellg();
        char* buff = new char[size];
        ROM_file.seekg(0, std::ios::beg);
        ROM_file.read(buff, size);
        ROM_file.close();
        for(int i =0;i<size;i++){
            memory[START_ADDR + i] = buff[i];
        }
        delete buff;
    }
}
Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count()),
      randByte(0, 255)   // range: 0–255
{
    pc = START_ADDR;

    for (int i = 0; i < FONTSET_SIZE; i++) {
        memory[FONT_SET_START_ADDR + i] = fontset[i];
    }
}

void Chip8::OP_00E0(){
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE(){
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1nnn(){
    // the line below is used to extract the last 12 bits of a 16 but instruction this is called but masking
    uint16_t address = opcode & 0x0FFFu;

	pc = address;
}

void Chip8::OP_2nnn(){
    uint16_t address = opcode & 0x0FFFu;
    stack[sp] = pc;
    ++sp;
    pc = address;
}

void Chip8::OP_3xkk(){
    //this is used in a lot of things go through this once
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    if(registers[Vx] == byte){
        pc+=2;
    }

}

void Chip8::OP_4xkk(){
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    if(registers[Vx] != byte){
        pc+=2;
    }
}

void Chip8::OP_5xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if(registers[Vx] == registers[Vy]){
        pc +2; 
    }
}

void Chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;
}

void Chip8::OP_7xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] += byte;
}

void Chip8::OP_8xy0(){
   uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; 
     registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1(){
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; 
     registers[Vx] = registers[Vx] | registers[Vy];
}

void Chip8::OP_8xy2(){
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; 
     registers[Vx] = registers[Vx] & registers[Vy];
}

void Chip8::OP_8xy3(){
   uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u; 
     registers[Vx] = registers[Vx] ^ registers[Vy];
}

void Chip8::OP_8xy4(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
     uint16_t sum  = registers[Vx] + registers[Vy];
     if(sum > 255U){
        registers[0xF] = 1;
     }
     else{
        registers[0xF] = 0;
     }
     registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5(){
    
       uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  
     if(registers[Vx] > registers[Vy]){
        registers[0xF] = 1;
     }
     else{
        registers[Vx] = 0; 
     }
        uint16_t sub =registers[Vx] - registers[Vy];
        registers[Vx] = sub; 
}

void Chip8::OP_8xy6(){
      uint8_t Vx = (opcode & 0x0F00u) >> 8u;
      //this is to find the lsb
      	registers[0xF] = (registers[Vx] & 0x1u);
        // this is the diviision by 2 operator mate
        	registers[Vx] >>= 1;
}

void Chip8::OP_8xy7(){
       uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     uint8_t Vy = (opcode & 0x00F0u) >> 4u;
     if(registers[Vy] > registers[Vx]){
      registers[0xF] = 1;
     }
     else{

      registers[0xF] = 0;
     }
     registers[Vx] -= registers[Vy];
} 

void Chip8::OP_8xyE(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}
void Chip8::OP_9xy0(){
uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if(registers[Vx] != registers[Vy]){
        pc +=2;
    }
}

void Chip8::OP_Annn(){
    uint16_t address = opcode & 0x0FFFu;

	index = address;
}

void Chip8::OP_Bnnn(){
    uint16_t address = opcode & 0x0FFu;
    pc = registers[0] + address;
}

void Chip8::OP_Cxkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}