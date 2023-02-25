//
// Created by Angel on 1/23/2023.
//
#include "chip8.h"



/*
    Chip8 - initializes the display buffer and load the font as well as the window
*/
Chip8::Chip8(const char * filename, int scale, int cycleDelayMS) : consoleInterface("Chip-8", WIDTH, HEIGHT, scale) {
    //set our clock speed from delay
    this->delay = cycleDelayMS;


    //Initialize the Display buffer
    DisplayBuffer = new bool * [HEIGHT];
    for(int i = 0; i < HEIGHT; i++)
        DisplayBuffer[i] = new bool [WIDTH];

    Op00E0(); //set to empty screen


    //initialize the SP, PC, and I pointers
    PC = STARTING_ADDR;
    SP = 0;
    I = 0;


    loadFont(MEMORY_BUFF, FONT_STARTING_ADDRESS, MEMORY_BUFF_SIZE);
}


Chip8::~Chip8(){
    for(int i = 0; i < HEIGHT; i++)
        delete [] DisplayBuffer[i];

    delete [] DisplayBuffer;
}




/*
    loadROM - loads ROM into memory buffer
    string filename
    Return Value : boolean
        true - file is successfully loaded into memory
        false - file cannot be successfully loaded into memory - check is done in
                function to see if ROM is larger than our memory buffer
*/
bool Chip8::loadROM(std::string filename) {

     std::ifstream ROM(filename, std::ifstream::in | std::ifstream::binary);

     if(!ROM.is_open()){
         std::cout << "Error: File Does not exist!\n";
         return false;
     }
     //get filesize
     auto begin = ROM.tellg();
     ROM.seekg(0, std::ios::end);
     auto end = ROM.tellg();
     auto ROMSize = end - begin;

     //compare with our buffer size, if we cannot fit, return false
     if(ROMSize > MEMORY_BUFF_SIZE) {
         std::cout << "Error: ROM File Size is too large!!!\n\n";
         return false;
     }

     //reset our position pointer and clear any fail bits if it triggers :p
     ROM.clear();
     ROM.seekg(0);

     ROM.read((char*)MEMORY_BUFF+STARTING_ADDR, ROMSize);
     ROM.close();

     ROM_loaded = true;
     return ROM_loaded;

}




/*
loadFont:
    Parameters:
        int8_t * RAM -> Main Memory Buffer to store data into
        int size -> size of Buffer to store data into
    Return Value : None
    Description:
        Function loads Font data from FONT buffer into Memory.
*/
void Chip8::loadFont(uint8_t * MEMORY_BUFFER, int start_address, int size){
    //if(FONT_SIZE > size-start_address) throw "FATAL ERROR: MEMORY BUFFER SIZE NOT ALLOTED ENOUGH SPACE FOR FONT";

    for(int i = 0; i < FONT_SIZE; i++) {
        MEMORY_BUFFER[i+start_address] = FONT_BUFF[i];
    }

}




void Chip8::run(){
    auto time_point =  std::chrono::steady_clock::now();
    PC = STARTING_ADDR;
    while(true) {
        if(consoleInterface.recordInput(keypad)) break;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_point).count() > delay){
            cycle();
            consoleInterface.renderDisplay(DisplayBuffer);
            time_point = std::chrono::steady_clock::now();
        }
    }
}















void Chip8::cycle() {
    //fetch
    opcode = (MEMORY_BUFF[PC] << 8) | MEMORY_BUFF[PC+1];
    PC+=2;

    //decode
    void(Chip8::*opFunctionPtr)() = nullptr;
    switch(opcode >> 12){
        case 0:
            switch(opcode & 0x00FF){
                case 0xE0: opFunctionPtr =  &Chip8::Op00E0; break;
                case 0xEE: opFunctionPtr =  &Chip8::Op00EE; break;
                default: opFunctionPtr = &Chip8::Op0nnn; break;
            }
            break;

        case 1: opFunctionPtr = &Chip8::Op1nnn; break;
        case 2: opFunctionPtr = &Chip8::Op2nnn; break;
        case 3: opFunctionPtr = &Chip8::Op3xkk; break;
        case 4: opFunctionPtr = &Chip8::Op4xkk; break;
        case 5: if(opcode % 16 == 0) opFunctionPtr = &Chip8::Op5xy0; break;
        case 6: opFunctionPtr = &Chip8::Op6xkk; break;
        case 7: opFunctionPtr = &Chip8::Op7xkk; break;

        case 8:
            switch(opcode & 0x000F) {
                case 1: opFunctionPtr   =   &Chip8::Op8xy1; break;
                case 0: opFunctionPtr   =   &Chip8::Op8xy0; break;
                case 2: opFunctionPtr   =   &Chip8::Op8xy2; break;
                case 3: opFunctionPtr   =   &Chip8::Op8xy3; break;
                case 4: opFunctionPtr   =   &Chip8::Op8xy4; break;
                case 5: opFunctionPtr   =   &Chip8::Op8xy5; break;
                case 6: opFunctionPtr   =   &Chip8::Op8xy6; break;
                case 7: opFunctionPtr   =   &Chip8::Op8xy7; break;
                case 0xE: opFunctionPtr =   &Chip8::Op8xyE; break;
            }
            break;

        case 9: if(opcode % 16 == 0) opFunctionPtr = &Chip8::Op9xy0; break;
        case 0xA: opFunctionPtr = &Chip8::OpAnnn; break;
        case 0xB: opFunctionPtr = &Chip8::OpBnnn; break;
        case 0xC: opFunctionPtr = &Chip8::OpCxkk; break;
        case 0xD: opFunctionPtr = &Chip8::OpDxyn; break;

        case 0xE:
            switch(opcode & 0x00FF){
                case 0x9E: opFunctionPtr = &Chip8::OpEx9E; break;
                case 0xA1: opFunctionPtr = &Chip8::OpExA1; break;
            }
            break;

        case 0xF:
            switch(opcode & 0x00FF) {
                case 0x07: opFunctionPtr = &Chip8::OpFx07; break;
                case 0x0A: opFunctionPtr = &Chip8::OpFx0A; break;
                case 0x15: opFunctionPtr = &Chip8::OpFx15; break;
                case 0x18: opFunctionPtr = &Chip8::OpFx18; break;
                case 0x1E: opFunctionPtr = &Chip8::OpFx1E; break;
                case 0x29: opFunctionPtr = &Chip8::OpFx29; break;
                case 0x33: opFunctionPtr = &Chip8::OpFx33; break;
                case 0x55: opFunctionPtr = &Chip8::OpFx55; break;
                case 0x65: opFunctionPtr = &Chip8::OpFx65; break;
            }
            break;

    }

    //execute

    //if nullptr, then throw an error for bad opcode
    if(opFunctionPtr != nullptr) ((*this).*opFunctionPtr)();
    else {
        std::stringstream ss;
        ss << std::hex << opcode;
        std::string errorMsg(ss.str());
        errorMsg = "FATAL ERROR : BAD OPCODE 0x"+errorMsg;
        std::cout << errorMsg << std::endl;
    }


    if(delay_timer > 0) {
        delay_timer--;
    }

    if(sound_timer > 0) {
        sound_timer--;
    }

}


//Opcode instructions
/*
*/
void Chip8::Op0nnn() {
    uint16_t nnn = opcode & 0x0FFF;
    PC = nnn;
}


/*
Op 00E0 - Clears the display buffer
*/
void Chip8::Op00E0 () {
    for(int i = 0; i < HEIGHT; i++)
        for(int j = 0; j < WIDTH; j++)
            DisplayBuffer[i][j] = false;
}

/*
Op00EE - RET
Returns from a subroutine, throws error if stack pointer is less than 0
*/
void Chip8::Op00EE() {
    //SP < 1  ? throw "FATAL ERROR : NO RETURN ADDRESS IN MEMORY" : PC = RA_Stack[--SP];
    if(SP < 1) {
        std::cout << "ERROR : NO RETURN ADDRESS IN MEMORY!\n";
    }
    PC = RA_Stack[--SP];
}


/*
Op 1nnn - Jumps to location nnn by setting program counter to that
*/
void Chip8::Op1nnn() {
    Op0nnn();
}

/*
Op 2nnn - Calls subroutine at address nnn
    Stack pointer at the current memory address to store the return address,
    thus we push then increment our stack pointer
*/
void Chip8::Op2nnn() {
    uint16_t nnn = opcode & 0x0FFF;
    RA_Stack[SP++] = PC;
    PC = nnn;
}

/*
Op 3xkk - Skip if equal
Compares  register Vx to kk, if they're equal than skip (PC+=2) the next
instruction
*/
void Chip8::Op3xkk(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t kk = opcode & 0x00FF;

    if(Vx == kk) PC+=2;
}

/*
Op 4xkk - Skip if not equal
Compares register Vx to kk, if they're not equal than skip (PC+=2) the next
instruction
*/
void Chip8::Op4xkk(){

    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t kk = opcode & 0x00FF;

    if(Vx != kk) PC+=2;
}


/*
Op 5xy0 - Skip Next instruction if Vx == Vy
*/
void Chip8::Op5xy0(){

    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];

    if(Vx == Vy) PC+=2;

}

/*
Op 6xkk - LD Vx, Byte
Loads kk into Vx register
*/
void Chip8::Op6xkk(){
    //Vx = kk
    registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
}

/*
Op 7xkk - Adds Vx and kk and stores value into Vx.
    Does not account for overflow
*/
void Chip8::Op7xkk(){
    registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
}

/*
Op 8xy0 - Assigns Vx = Vy
*/
void Chip8::Op8xy0(){
    registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
}         //LD Vx, Vy

/*
Op 8xy1 - Assignns Vx = Vx OR Vy
*/
void Chip8::Op8xy1(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];

    registers[(opcode & 0x0F00) >> 8] = Vx | Vy ;
}
//OR Vx, Vy

/*
Op 8xy2 - Assigns Vx = Vx AND Vy
*/
void Chip8::Op8xy2(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];

    registers[(opcode & 0x0F00) >> 8] = Vx & Vy;
}

/*
Op 8xy3 - Assigns Vx = Vx XOR Vy
*/
void Chip8::Op8xy3(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];

    registers[(opcode & 0x0F00) >> 8] = Vx ^ Vy;
}

/*
Op 8xy4 Assigns Vx = Vx + Vy , sets VF as the carry register
*/
void Chip8::Op8xy4(){
    //resultant variables upper 8 bits store carry , lower 8 store result
    uint16_t Vf_Vx;

    //operands Vx + Vy
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];


    Vf_Vx = Vx + Vy;

    registers[(opcode & 0x0F00) >> 8] = (Vf_Vx & 0x00FF); //Vx assignment

    registers[0xF] = (Vf_Vx & 0x0F00) >> 8;               //Vf assignment

}

/*
Op 8xy5 - Assigns Vx = Vx - Vy , If Vx > Vy assign VF to 1, else 0 ;
*/
void Chip8::Op8xy5(){

    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];

    Vx > Vy ? registers[0xF] = 1 : registers[0xF] = 0 ;
    registers[(opcode & 0x0F00) >> 8 ] = Vx - Vy;
}

//Shift Right Vx by 1, Vy not significant
void Chip8::Op8xy6() {
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];

    //check if LSB is 1, then set VF as carry out
    (Vx & 0x01) ? registers[0xF] = 1 : registers[0xF] = 0;
    registers[(opcode & 0x0F00) >> 8] = Vx >> 1;
}

/*
Op 8xy7 - Assigns Vx = Vy - Vx. If Vy > Vx, VF = 1, else VF = 0
*/
void Chip8::Op8xy7(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];

    //check if underflow, set 1 if true , else set 0
    Vy > Vx ? registers[0xF] = 1 : registers[0xF] = 0;

    //Vx = Vy - Vx;
    registers[(opcode & 0x0F00) >> 8] = Vy - Vx;
}         //SUBN Vx, Vy

/*
Op 8xyE - Shift Left Vx by 1, If it overflows, set VF to 1, else VF = 0
*/
void Chip8::Op8xyE(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    (Vx & 0x80) ? registers[0xF] = 1 : registers[0xF] = 0;  //Save overflow to VF
    registers[(opcode & 0x0F00) >> 8] = Vx << 1;
}

/*
Op 9xy0 - Skip Next instruction (pc += 2 ) if Vx != Vy
*/
void Chip8::Op9xy0(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4];

    if(Vx != Vy) PC+=2;

}

/*
Op Annn - Register I is set to nnn
*/
void Chip8::OpAnnn(){
    uint16_t nnn = opcode & 0x0FFF;
    I = nnn;
}

/*
Op Bnnn - Jumps to address nnn + V0 value
*/
void Chip8::OpBnnn(){
    // PC = nnn + V0
    PC = (opcode & 0x0FFF) + registers[0];
}

/*
Op Cxkk - Vx = [random byte] & kk
*/
void Chip8::OpCxkk(){

    registers[(opcode & 0x0F00) >> 8] = (std::rand() & (opcode & 0x00FF));
}

/*
Op Dxyn - Draw n-bytes of a sprite at position (Vx, Vy) located at address stored
    on I
*/
void Chip8::OpDxyn(){

    uint8_t Vx = registers[(opcode & 0x0F00) >> 8]; //x coord
    uint8_t Vy = registers[(opcode & 0x00F0) >> 4]; //y coord
    uint8_t n = opcode & 0x000F;                    //bytes to draw

    uint8_t pixel = 0x80; // pixel to AND with our byte to draw to screen


    registers[0xF] = 0; //reset flag to 0 as it will stand if we have sprite overlay
    short offset = 0;

    while(n>0) {
        //if we have drawn the full byte, reset our x coord, increment our Y coord and
        //memory pointer, and reset our pixel
        if(pixel == 0) {
            Vy += 1;
            offset+=1;
            pixel = 0x80;
            Vx = registers[(opcode & 0x0F00) >> 8];
            n--;
        }
        else {
            if(DisplayBuffer[Vy % HEIGHT][Vx % WIDTH] && (MEMORY_BUFF[I+offset] & pixel))
                registers[0xF] = 1;

            DisplayBuffer[Vy % HEIGHT][Vx % WIDTH] ^= (bool)(MEMORY_BUFF[I+offset] & pixel);
            Vx++;
            pixel >>= 1;
        }
    }

}

/*
Op Ex9E - Skips the next instruction (PC+=2) if keypad with the value
          stored in Vx is currently pressed
*/
void Chip8::OpEx9E(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];

    if(keypad[Vx]) PC+=2;
}

/*
Op ExA1 - skips the next instruction if the value of Vx is NOT pressed
*/
void Chip8::OpExA1(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];

    if(!keypad[Vx]) PC +=2;
}

/*
 Op Fx07 - Loads the value of the delay timer (DT) into
*/
void Chip8::OpFx07() {
    registers[(opcode & 0x0F00) >> 8] = delay_timer;
}

/*
Op Fx0A - Stall until a keypress is detected. Whichever key is pressed is stored
          into register Vx
*/
void Chip8::OpFx0A(){
    bool anyKeyPressed = false;
    for(uint8_t i = 0; i < 16; i++) {
        if(keypad[i]){
            registers[(opcode & 0x0F00) >> 8] = i;
            anyKeyPressed = true;
        }
    }
    if(!anyKeyPressed) PC-=2;
}

/*
Op Fx15 - Set the Delay Timer to Vx
*/
void Chip8::OpFx15(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    delay_timer = Vx;
}

/*
Op Fx18 - Set the Sound Timer to Vx
*/
void Chip8::OpFx18(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    sound_timer = Vx;
}

/*
Op Fx1E - Assigns I = I + Vx
*/
void Chip8::OpFx1E(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];
    I += Vx;
}

/*
Op Fx29 - Assigns I for the location of sprite for Digit in Vx
*/
void Chip8::OpFx29(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];

    //Digit sprites are 8x5 hence Vx * 5
    I = FONT_STARTING_ADDRESS + (Vx * 5);
}

/*
Op Fx33 - Stores Vx value in BCD Format. As Vx registers are 8 bit and are ranged
          0 - 255 we follow this mapping

          I      -> stores 100s digit
          I + 1  -> stores 10s digit
          I + 2  -> stores 1s digit
*/
void Chip8::OpFx33(){
    uint8_t Vx = registers[(opcode & 0x0F00) >> 8];

    for(int offset = 2; offset >= 0; offset--){
        MEMORY_BUFF[I+offset] = Vx % 10;
        Vx /= 10;
    }
}

/*
Op Fx55 - Stores Register V0 through Vx (inclusive) in memory starting at I;
*/
void Chip8::OpFx55(){
    uint8_t x = (opcode & 0x0F00) >> 8;


    for(uint8_t j = 0; j <= x; j++) {
        MEMORY_BUFF[I+j] = registers[j];
    }
}

/*
Op Fx65 - Loads from I to I+x into registers V0 - Vx (inclusive)
*/
void Chip8::OpFx65(){
    uint8_t x = (opcode & 0x0F00) >> 8;

    for(uint8_t j = 0; j <= x; j++) {
        registers[j] = MEMORY_BUFF[I+j];
    }
}

void Chip8::decrementTimers(){
    while(running) {
        Sleep(16);
        if(delay_timer > 0) delay_timer--;
        if(sound_timer > 0) sound_timer--;
    }
}
