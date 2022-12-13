#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();
SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gHelloWorld = NULL;

int main(int argc, char* args[]) {
    if(!init()) {
        std::cout << "Didn't initialize" << "\n";
    }else {
        if(!loadMedia()) {
            std::cout << "Media not loaded" << "\n";
        }else {
            SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
            SDL_UpdateWindowSurface(gWindow);
            SDL_Event e;
            bool quit = false;
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }
            }
        }
    }

    close();
    return 0;
}

bool init() {
    bool success = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Error initializing: " << SDL_GetError() << "\n";
        success = false;
    }
    else {
        gWindow = SDL_CreateWindow("Learning SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL) {
            std::cout << "Window not created error: " << SDL_GetError() << "\n";
            success = false;
        }else {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }
    return success;
}

bool loadMedia() {
    bool success = true;
    gHelloWorld = SDL_LoadBMP("C:/Users/davro/Documents/snail.bmp");
    if(gHelloWorld == NULL) {
        std::cout << "Image not loaded, error: " << SDL_GetError() << "\n";
        success = false;
    }
    return success;
}

void close() {
    SDL_FreeSurface(gHelloWorld);
    gHelloWorld = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    SDL_Quit();
}

