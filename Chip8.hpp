#pragma once

#include <cstdint>

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

class Chip8 {
    private:
        uint8_t memory[MEMORY_SIZE]{};
        uint8_t registers[REGISTER_COUNT]{};
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[STACK_LEVELS]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint16_t opcode{};
    public:
        uint16_t keypad[KEY_COUNT]{};
        uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};
        Chip8();
        void LoadRom(const char* filename);
}