//
// Created by Angel on 1/23/2023.
//
#include "console_interface.h"

/*
    ConsoleInterface - Parameter Constructor initializes SDL subsystems, window and renderer.
    Throws error if SDL systems can't be initialized

    Parameters :
        const char * windowName,
        int WIDTH,
        int HEIGHT,
        int SCALE,

    Return Value : None
*/
ConsoleInterface::ConsoleInterface(const char * windowName, int WIDTH, int HEIGHT, int SCALE ){

    //check if we can initialize SDL Subsystems
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("ERROR: SDL Could not initialize");


    //assign window parameters
    this->WIDTH = WIDTH;
    this->HEIGHT = HEIGHT;
    this->SCALE = SCALE;



    //create window and renderer and make it an empty black screen.
    SDL_CreateWindowAndRenderer(WIDTH*SCALE, HEIGHT*SCALE, 0, &gameWindow, &gameRenderer);

    SDL_RenderSetScale(gameRenderer, SCALE, SCALE);

    SDL_SetRenderDrawColor(gameRenderer, 0,0,0,255);
    SDL_RenderClear(gameRenderer);
    SDL_RenderPresent(gameRenderer);

}

/*
    ~ConsoleInterface - Destructor that destroyws window + renderer and halts SDL subsystems
*/
ConsoleInterface::~ConsoleInterface(){
    SDL_DestroyRenderer(gameRenderer);
    SDL_DestroyWindow(gameWindow);
    SDL_Quit();
}

/*
    void RenderDisplay - display the current state of the displayArray to the window
*/
void ConsoleInterface::renderDisplay(bool ** buff) {
    SDL_SetRenderDrawColor(gameRenderer,0,0,0, 255); //black
    SDL_RenderClear(gameRenderer);
    SDL_SetRenderDrawColor(gameRenderer, 255, 255, 255, 255); //white

    //set pixels from buffer
    for(int y = 0; y < HEIGHT; y++)
        for(int x = 0; x < WIDTH; x++) {
            if(buff[y][x]) SDL_RenderDrawPoint(gameRenderer, x, y);
        }

    //present final state of renderer
    SDL_RenderPresent(gameRenderer);
}

/*
    bool ConsoleInterface::recordInput - stores the state of the keypad presses
    Arguments:
        bool * keypad - pointer to array of keypad flags
    Return Value:
        Boolean:
            True - Stop Console
            False - Continue;
*/
bool ConsoleInterface::recordInput(bool * keypad) {
    bool halt = false;
    SDL_Event e;

    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT){
            halt = true;
        }

        switch(e.type) {
            case SDL_QUIT:
                halt = true;
                break;

            case SDL_KEYDOWN:
                //dont count non mapped keys
                if(e.key.keysym.sym == SDLK_ESCAPE) halt = true;
                else if(e.key.keysym.sym != SDLK_0 && keypadMapping[e.key.keysym.sym] == 0 ) break;

                //casting fixes ? lol
                keypad[keypadMapping[e.key.keysym.sym]] = true;
                    std::cout << std::hex <<  keypadMapping[e.key.keysym.sym] << std::endl;

                for(int i = 0 ; i < 16; i++) {

                    std::cout << keypad[i] << " ";
                    if((i%4) == 3) std::cout << std::endl;
                }


                break;


            case SDL_KEYUP:
                    std::cout << std::hex << keypadMapping[e.key.keysym.sym] << std::endl;

                //Don't count non mapped keys
                if(e.key.keysym.sym != SDLK_0 && keypadMapping[e.key.keysym.sym] == 0 ) break;

                keypad[keypadMapping[e.key.keysym.sym]] = false;
                break;
        }

    }
    return halt;
}