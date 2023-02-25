//
// Created by Angel on 1/23/2023.
//

#ifndef SDLTEST_CONSOLE_INTERFACE_H
#define SDLTEST_CONSOLE_INTERFACE_H

#include <iostream>
#include <unordered_map>
#include <SDL2/SDL.h>



//define keyboard mapping - default
static std::unordered_map<int, int> keypadMapping = {
        /*

        1  2  3  4       1  2  3  C
        q  w  e  r       4  5  6  D
        a  s  d  f  - >  7  8  9  E
        z  x  c  v       A  0  B  F

        */
        {SDLK_1, 0x1},
        {SDLK_2, 0x2},
        {SDLK_3, 0x3},
        {SDLK_4, 0xC},

        {SDLK_q, 0x4},
        {SDLK_w, 0x5},
        {SDLK_e, 0x6},
        {SDLK_r, 0xD},

        {SDLK_a, 0x7},
        {SDLK_s, 0x8},
        {SDLK_d, 0x9},
        {SDLK_f, 0xE},

        {SDLK_z, 0xA},
        {SDLK_x, 0x0},
        {SDLK_c, 0xB},
        {SDLK_v, 0xF},

};



class ConsoleInterface{
public:
    ConsoleInterface(const char * windowName, int WIDTH, int HEIGHT, int SCALE);
    ~ConsoleInterface();
    void renderDisplay(bool ** buff);
    bool recordInput(bool * keypad);

private:
    SDL_Window * gameWindow = nullptr;
    SDL_Renderer * gameRenderer = nullptr;
    //SDL_Texture * texture = nullptr;

    int WIDTH;
    int HEIGHT;
    int SCALE;


};

#endif
