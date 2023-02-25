//
// Created by Angel on 1/23/2023.
//

#ifndef SDLTEST_CHIP8_H
#define SDLTEST_CHIP8_H


#include <stdint.h>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <Windows.h>
#include <thread>
#include <sstream>
#include "font.h"
#include "console_interface.h"


/*
Note: opcodes are following the naming convention from this resource : http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
which describe in detail that they follow this naming convention with direct quotation:
    nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
    n or nibble - A 4-bit value, the lowest 4 bits of the instruction
    x - A 4-bit value, the lower 4 bits of the high byte of the instruction
    y - A 4-bit value, the upper 4 bits of the low byte of the instruction
    kk or byte - An 8-bit value, the lowest 8 bits of the instruction

    - Section 3.0 - Chip-8 Instructions
*/


/*
+---------------+= 0xFFF (4095) End of Chip-8 RAM
|               |
|               |
|               |
|               |
|               |
| 0x200 to 0xFFF|
|     Chip-8    |
| Program / Data|
|     Space     |
|               |
|               |
|               |
+- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
|               |
|               |
|               |
+---------------+= 0x200 (512) Start of most Chip-8 programs
| 0x000 to 0x1FF|
| Reserved for  |
|  interpreter  |
+---------------+= 0x000 (0) Start of Chip-8 RAM
*/


/*
Stack - Chip 8 programs use it only to store return addresses when they call functions
Stores 16-bit values , with a static size of 16 memory cells to occupy
+---------------+=0xF (15) End of Stack Array
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
|               |
+---------------+= 0x0 <- SP initialized to here
*/

const unsigned int MEMORY_BUFF_SIZE = 4096;
const unsigned int STARTING_ADDR = 0x200;      //starting Address for programs
const uint16_t STACK_SIZE = 16;     //stack size used for storing return addresses
const unsigned int FONT_STARTING_ADDRESS = 0x50;

//Dimensions
const int HEIGHT = 32;
const int WIDTH = 64;




class Chip8{
public:

    Chip8(const char * filename, int scale, int cyclemsDelay);
    ~Chip8();


    bool loadROM(std::string filename);
    static void loadFont(uint8_t * MEMORY_BUFF, int start_address, int size);

    void run();
    bool running = false;

    //Display to draw to the screen to
    bool ** DisplayBuffer = nullptr;
    //bool DisplayBuffer[32][64] = {0};
    bool keypad[16] = {0}; // keypad to handle input

    //Timers - 60 Hz
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;
    void decrementTimers();     //maybe


    //Display and Console interface
    ConsoleInterface consoleInterface;




private:

    //fetch decode execute cyle and the cycle delay
    void cycle();
    int delay = 0;

    //BUFFERS
    uint8_t MEMORY_BUFF [MEMORY_BUFF_SIZE] = {0};   //memory
    uint16_t RA_Stack[STACK_SIZE];                  //Return Address Stack



    //REGISTERS
    uint8_t registers[16] = {};                         //General Purpose
    uint8_t SP = 0;                                     //Stack Pointer
    uint16_t PC = STARTING_ADDR;                        //Program Counter
    uint16_t I = 0;                                     //Memory Index Register - only 12 bit


    //flag to get if rom loaded
    bool ROM_loaded = false;



    //opcode and opcode functions
    uint16_t opcode;

    void Op0nnn ();                                 //Sys Addr
    void Op00E0 ();                                 //CLS
    void Op00EE ();                                 //RET
    void Op1nnn ();                                 //JP addr
    void Op2nnn ();                                 //CALL addr
    void Op3xkk ();                                 //SE Vx, byte
    void Op4xkk ();                                 //SNE Vx, byte
    void Op5xy0 ();                                 //SE Vx, Vy
    void Op6xkk ();                                 //LD Vx, byte
    void Op7xkk ();                                 //ADD Vx, byte
    void Op8xy0 ();                                 //LD Vx, Vy
    void Op8xy1 ();                                 //OR Vx, Vy
    void Op8xy2 ();                                 //AND Vx, Vy
    void Op8xy3 ();                                 //XOR Vx, Vy
    void Op8xy4 ();                                 //ADD Vx, Vy
    void Op8xy5 ();                                 //Sub Vx, Vy
    void Op8xy6 ();                                 //Shift Right Vx by 1, Vy not significant
    void Op8xy7 ();                                 //SUBN Vx, Vy
    void Op8xyE ();                                 //Shift Left Vx by 1, Vy not significant
    void Op9xy0 ();                                 //SNE Vx, Vy
    void OpAnnn ();                                 //LD I, addr
    void OpBnnn ();                                 //JP V0, Addr
    void OpCxkk ();                                 //RND Vx, byte
    void OpDxyn ();                                 //DRW Vx, Vy, nibble
    void OpEx9E ();                                 //SKP Vx
    void OpExA1 ();                                 //SKNP Vx'
    void OpFx07 ();                                 //LD Vx, Dt
    void OpFx0A ();                                 //LD Vx, K
    void OpFx15 ();                                 //LD Dt, Vx
    void OpFx18 ();                                 //LD ST, Vx
    void OpFx1E ();                                 //Add I, Vx
    void OpFx29 ();                                 //LD F, Vx
    void OpFx33 ();                                 //LD B, Vx
    void OpFx55 ();                                 //LD [I], Vx
    void OpFx65 ();                                 //LD Vx, [I]



};


#endif //SDLTEST_CHIP8_H
