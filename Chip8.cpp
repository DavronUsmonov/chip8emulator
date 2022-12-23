#include "./Chip8.hpp"
#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>
#include <cstring>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
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

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    //Load fonts
    for(unsigned int i = 0; i < FONTSET_SIZE; i++) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
    //Set program counter to 0x200
    pc = START_ADDRESS; 

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk; 
    table[0x5] = &Chip8::OP_5xy0; 
    table[0x6] = &Chip8::OP_6xkk;   
    table[0x7] = &Chip8::OP_7xkk; 
    table[0x8] = &Chip8::Table8; 
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;
    //set all indexes in subtables to OP_NULL in case one is accidently accessed.
    for(size_t i = 0; i <= 0xE; i++) {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }
    for(size_t i = 0; i <= 0x65; i++) {
        tableF[i] = &Chip8::OP_NULL;
    }
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0xE] = &Chip8::OP_Ex9E;
    tableE[0x1] = &Chip8::OP_ExA1;

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::Cycle() {
    opcode = (memory[pc] << 8u) | memory[pc+1];
    pc += 2;

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
    if(delayTimer > 0) {
        --delayTimer;
    }
    if(soundTimer > 0) {
        --soundTimer;
    }
}

void Chip8::LoadRom(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open()) {
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        
        for(long i = 0; i < size; i++) {
            memory[START_ADDRESS+i] = buffer[i];
        }
        delete[] buffer;
    }
}

void Chip8::Table0() {
    ((*this).*table0[opcode & 0x000Fu])();
}

void Chip8::Table8() {
    ((*this).*table8[opcode & 0x000Fu])();
}

void Chip8::TableE() {
    ((*this).*tableE[opcode & 0x000Fu])();
}

void Chip8::TableF() {
    ((*this).*tableF[opcode & 0x00FFu])();
}

void Chip8::OP_NULL() {

}

void Chip8::OP_00E0() {
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE() {
    sp--;
    pc = stack[sp];
}

void Chip8::OP_1nnn() {
    uint16_t newAddress = opcode & 0x0FFFu;
    pc = newAddress;
}

void Chip8::OP_2nnn() { 
    stack[sp] = pc;
    sp++;
    uint16_t newAddress = opcode & 0x0FFFu;
    pc = newAddress;
}

void Chip8::OP_3xkk() {
    uint8_t currRegister = (opcode & 0x0F00u) >> 8u;
    uint8_t comp = opcode & 0x00FFu;
    if(registers[currRegister] == comp) pc += 2;
}

void Chip8::OP_4xkk() {
    uint8_t currRegister = (opcode & 0x0F00u) >> 8u;
    uint8_t comp = opcode & 0x00FFu;
    if(registers[currRegister] != comp) pc += 2;
}

void Chip8::OP_5xy0() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    if(registers[reg1] == registers[reg2]) pc += 2;
}

void Chip8::OP_6xkk() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t comp = opcode & 0x00FFu;
    registers[reg] = comp;
}

void Chip8::OP_7xkk() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t comp = opcode & 0x00FFu;
    registers[reg] += comp;
}

void Chip8::OP_8xy0() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    registers[reg1] = registers[reg2];
}

void Chip8::OP_8xy1() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    registers[reg1] = registers[reg1] | registers[reg2];
}

void Chip8::OP_8xy2() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    registers[reg1] = registers[reg1] & registers[reg2];
}

void Chip8::OP_8xy3() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    registers[reg1] = registers[reg1] ^ registers[reg2];
}

void Chip8::OP_8xy4() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = registers[reg1] + registers[reg2];
    registers[0xF] = (sum > 0xFFu) ? 1 : 0;
    registers[reg1] = sum & 0xFFu;
}

void Chip8::OP_8xy5() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    registers[0xF] = (registers[reg1] > registers[reg2]) ? 1 : 0;
    registers[reg1] -= registers[reg2];
}

void Chip8::OP_8xy6() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = registers[reg] & 0x1u;
    registers[reg] >>= 1;
}

void Chip8::OP_8xy7() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    registers[0xF] = (registers[reg2] > registers[reg1]) ? 1 : 0;
    registers[reg1] = registers[reg2] - registers[reg1];
}

void Chip8::OP_8xyE() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = (registers[reg] & 0x80) >> 8u;
    registers[reg] <<= 1;
}

void Chip8::OP_9xy0() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    if(registers[reg1] != registers[reg2]) pc += 2;
}

void Chip8::OP_Annn() {
    uint16_t newAdd = (opcode & 0x0FFFu);
    index = newAdd;
}

void Chip8::OP_Bnnn() {
    uint16_t newAdd = (opcode & 0x0FFFu);
    pc = newAdd + registers[0];
}

void Chip8::OP_Cxkk() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u; 
    uint8_t byte = opcode & 0x00FFu;
    registers[reg] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn() {
    uint8_t reg1 = (opcode & 0x0F00u) >> 8u;
    uint8_t reg2 = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;
    uint8_t xPos = registers[reg1] % VIDEO_WIDTH;
    uint8_t yPos = registers[reg2] % VIDEO_HEIGHT;
    registers[0xF] = 0;
    for(unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[index+row];
        for(unsigned int col = 0; col < 8; ++col) {
            uint8_t currSpritePixel = spriteByte & (0x80u >> col);
            uint32_t* videoPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            if(currSpritePixel) {
                if(*videoPixel == 0xFFFFFFFF) registers[0xF] = 1;
                *videoPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_Ex9E() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[reg];
    if(keypad[key]) pc += 2;
}

void Chip8::OP_ExA1() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[reg];
    if(!keypad[key]) pc += 2;
}

void Chip8::OP_Fx07() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    registers[reg] = delayTimer;
}

void Chip8::OP_Fx0A() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    if(keypad[0]) registers[reg] = 0;
    else if(keypad[1]) registers[reg] = 1;
    else if(keypad[2]) registers[reg] = 2;
    else if(keypad[3]) registers[reg] = 3;
    else if(keypad[4]) registers[reg] = 4;
    else if(keypad[5]) registers[reg] = 5;
    else if(keypad[6]) registers[reg] = 6;
    else if(keypad[7]) registers[reg] = 7;
    else if(keypad[8]) registers[reg] = 8;
    else if(keypad[9]) registers[reg] = 9;
    else if(keypad[10]) registers[reg] = 10;
    else if(keypad[11]) registers[reg] = 11;
    else if(keypad[12]) registers[reg] = 12;
    else if(keypad[13]) registers[reg] = 13;
    else if(keypad[14]) registers[reg] = 14;
    else if(keypad[15]) registers[reg] = 15;
    else pc -= 2;
}

void Chip8::OP_Fx15() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    delayTimer = registers[reg];
}

void Chip8::OP_Fx18() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    soundTimer = registers[reg];
}

void Chip8::OP_Fx1E() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    index += registers[reg];
}

void Chip8::OP_Fx29() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    index = FONTSET_START_ADDRESS + (registers[reg] * 5);
}

void Chip8::OP_Fx33() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[reg];
    memory[index] = (digit/100)%10;
    memory[index+1] = (digit/10)%10;
    memory[index+2] = digit%10;
}

void Chip8::OP_Fx55() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0; i <= reg; i++) {
        memory[index+i] = registers[i];
    }
}

void Chip8::OP_Fx65() {
    uint8_t reg = (opcode & 0x0F00u) >> 8u;
    for(uint8_t i = 0; i <= reg; i++) {
        registers[i] = memory[index + i];
    }
}