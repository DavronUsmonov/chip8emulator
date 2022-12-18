#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

//g++ -std=c++17 Platform.cpp -I"include" -L"lib" -Wall -lmingw32 -lSDL2main -lSDL2 -o Chip8Emulator

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum KeyPressSurfaces {
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

bool init();
bool loadMedia();
void close();
SDL_Surface *loadSurface(std::string path);
SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
SDL_Surface *gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface *gCurrentSurface = NULL;


int main(int argc, char* args[]) {
    if(!init()) {
        std::cout << "Didn't initialize" << "\n";
    }else {
        if(!loadMedia()) {
            std::cout << "Media not loaded" << "\n";
        }else {
            gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
            SDL_Event e;
            bool quit = false;
            while(!quit) {
                while(SDL_PollEvent(&e) != 0) {
                    SDL_FillRect(gScreenSurface, NULL, 0x000000);
                    SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
                    SDL_UpdateWindowSurface(gWindow);
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                    else if(e.type == SDL_KEYDOWN) {
                        switch( e.key.keysym.sym )
                        {
                            case SDLK_UP:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
                            break;

                            case SDLK_DOWN:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
                            break;

                            case SDLK_LEFT:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
                            break;

                            case SDLK_RIGHT:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
                            break;

                            default:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
                            break;
                        }
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
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT]= SDL_LoadBMP("C:/Users/davro/Documents/snail.bmp");
    if(gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
        std::cout << "Image not loaded, error: " << SDL_GetError() << "\n";
        success = false;
    }
    gKeyPressSurfaces[KEY_PRESS_SURFACE_UP]= SDL_LoadBMP("C:/Users/davro/Documents/greenland.bmp");
    if(gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL) {
        std::cout << "Image not loaded, error: " << SDL_GetError() << "\n";
        success = false;
    }
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN]= SDL_LoadBMP("C:/Users/davro/Documents/dots.bmp");
    if(gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL) {
        std::cout << "Image not loaded, error: " << SDL_GetError() << "\n";
        success = false;
    }
    gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT]= SDL_LoadBMP("C:/Users/davro/Documents/deer.bmp");
    if(gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL) {
        std::cout << "Image not loaded, error: " << SDL_GetError() << "\n";
        success = false;
    }
    gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT]= SDL_LoadBMP("C:/Users/davro/Documents/3bmp.bmp");
    if(gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL) {
        std::cout << "Image not loaded, error: " << SDL_GetError() << "\n";
        success = false;
    }
    return success;
}

void close() {
    SDL_FreeSurface(gCurrentSurface);
    gCurrentSurface = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    SDL_Quit();
}


SDL_Surface *loadSurface(std::string path) {
    SDL_Surface *loadedSurface = SDL_LoadBMP(path.c_str());
    if(loadedSurface == NULL) {
        std::cout << "Image can't load: " << path << SDL_GetError() << "\n";
    }
    return loadedSurface;
}
