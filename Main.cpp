#define SDL_MAIN_HANDLED

#include "./Platform.cpp"
#include "./Chip8.cpp"
#include <chrono>
#include <iostream>

//g++ -std=c++17 Main.cpp -I"include" -L"lib" -Wall -lmingw32 -lSDL2main -lSDL2 -o Chip8Emulator

int main(int argc, char** argv) {
    if(argc != 4) {
        std::cerr << "Usage: " << argv[0] << "<Scale> <Delay> <Rom>\n";
        std::exit(EXIT_FAILURE);
    }
    int videoScale = std::stoi(argv[1]);
    int delay = std::stoi(argv[2]);
    const char* fileName = argv[3];

    Platform platform("Chip8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);
    Chip8 chip8;
    chip8.LoadRom(fileName);
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while(!quit) {
        quit = platform.ProcessInput(chip8.keypad);
        auto currentTime = std::chrono::high_resolution_clock::now();
        float downtime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if(downtime > delay) {
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }
    return 0;
}