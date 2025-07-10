#ifndef __CHIP8_HEADER_FILE__
#define __CHIP8_HEADER_FILE__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"          // for showing window

#define SCREEN_WIDTH          64    // 32x64 is the native CHIP8 emulator reolution
#define SCREEN_HEIGHT         32    // so by default we have 64 x 32 pixels
#define SCALE_FACTOR          20    // after scaling 1280 x 640 pixels

/* EMULATOR CONGIGURATIONS */
#define EMULATOR_RAM          4096  // emulator has 4096 memory locations each
                                    // of size 8 bits (1 byte). Hence the name chip8
#define STACK_SIZE            12    // stack size - supports 12 level of nesting
#define DATA_REGISTER         16    // Chip8 has 16 - 8bit register (V0 to VF)
#define KEYS_COUNT            16    // has 16 keys 0 to F



/* build-in font for chip8 */
// each font character are 4 pixels wide and 5 pixels tall
//
// for example: consider the font character - 0
// 0xF0   11110000 
// 0x90   10010000
// 0x90   10010000      
// 0x90   10010000
// 0xF0   11110000
//
// fontsets are loaded in the RAM, anywhere in first 512 bytes. Index registor(I)
// is set to the character's memory location and it will be drawn in the screen
const uint8_t fontset[] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
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



/* status of the emulator */
typedef enum { STOP, RUNNING, PAUSED } emulator_status_t;



/* struct to hold the emulator configurations */
typedef struct {
  uint32_t win_width;      // SDL window width
  uint32_t win_height;     // SDL window height
  uint32_t scale_factor;   // to scale the win size 

  uint8_t r, b, g, a;      // background color (red, green, blue, alpha)

  char *rom;               // location to ROM file
} config_t;



/* struct to hold the SDL components */
typedef struct {
  SDL_Window *win;       // main SDL window pointer
  SDL_Renderer *render;  // 2D rendering context for a window 
} sdlc_t;



// for DISPLAY REFRESH we need to have individual bit corresponding to each
// pixel in the screen. 
// By default 256 bytes (from 0xF00 to 0xFFF) is reserved for display refresh
// so, 256 bytes x 8 = 2048 bits

// comparing this with display resolution:
// by default the screen size of CHIP8 emulator is 
// 64 x 32 -> 2048 total pixels

// since emulator has black and white display (monochrome), 
// single bit per pixel is fine. 1 - white & 0 - black

// STACK - used to store return address of functions
// support 12 levels of nesting. So, we need stack size of 12 to hold function address

// KEYPAD - total 16 keys from 0 to F
// keys 8,4,6,2 are for directional inputs

/* struct to hold the emulator description */
typedef struct {
  emulator_status_t status;       // state of the emulator

  // MEMORY
  uint8_t ram[EMULATOR_RAM];      // RAM size

  // REGISTER
  uint8_t V[DATA_REGISTER];       // data register V0 - VF, VF is carry flag
  uint16_t I;                     // address/index register
  uint16_t PC;                    // program counter

  // STACK
  uint16_t stack[STACK_SIZE];

  // TIMER
  uint8_t delay_timer;      // count at 60Hz, until reaches 0
  uint8_t sound_timer;      // count at 60Hz, until reaches 0. used for sound effects
                            // makes beeping sound, when value is non-zero
  
  // INPUT (keyboard)
  bool keypad[KEYS_COUNT];                     // hexadecimal keypad

  // DISPLAY REFRESH
  bool display[SCREEN_WIDTH * SCREEN_HEIGHT];  // each bits represent the state
                                               // of a single pixel (monochrome)

  // LOADED ROM (place where ROM is stored)
  char *rom;                 // currently running ROM
} emulator_t;






/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* ---------- FUNCTION PROTOTYPES ---------- */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */



/* ---------- UTIL FUNCTION PROTOTYPES ---------- */
bool load_rom(emulator_t *);

size_t rom_size(FILE *);

#endif   // __CHIP8_HEADER_FILE__