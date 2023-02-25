#include "Chip8.h"
#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    Chip8 console("Chip8", 15, 1);
    std::string ROM_Name;



    bool romLoaded = false;
    while(!romLoaded) {
        std::cout << "Enter ROM name in folders /ROMS\n\n";
        std::cin >> ROM_Name;
        romLoaded = console.loadROM("ROMS/" + ROM_Name);
    }

    console.run();
    return 0;
}