#ifndef __CHIP8_HEADER_FILE__
#define __CHIP8_HEADER_FILE__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"          // for showing window

#define CHIP8_NATIVE_WIDTH    64    // 32x64 is the native CHIP8 emulator reolution
#define CHIP8_NATIVE_HEIGHT   32    // so by default we have 64 x 32 pixels
#define SCALE_FACTOR          20    // after scaling 1280 x 640 pixels

/* EMULATOR CONGIGURATIONS */
#define EMULATOR_RAM          4096  // emulator has 4096 memory locations each
                                    // of size 8 bits (1 byte)
#define MAX_NESTING_LEVEL     12    // stack size
#define DATA_REGISTER         16    // Chip8 has 16 - 8bit register (V0 to VF)
#define KEYS_COUNT            16    // has 16 keys 0 to F



/* status of the emulator */
typedef enum { STOP, RUNNING, PAUSED } emulator_status_t;


/* struct to hold the emulator configurations */
typedef struct {
  uint32_t win_width;      // SDL window width
  uint32_t win_height;     // SDL window height
  uint32_t scale_factor;   // to scale the win size 

  uint8_t r, b, g, a;      // background color (red, green, blue, alpha)
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
  emulator_status_t status;   // state of the emulator

  // MEMORY
  uint8_t ram[EMULATOR_RAM];                               // RAM size
  bool display[CHIP8_NATIVE_WIDTH * CHIP8_NATIVE_HEIGHT];  // display refresh

  // REGISTER
  uint8_t V[DATA_REGISTER];       // data register V0 - VF
  uint16_t I;                     // address/index register
  uint16_t PC;                    // program counter

  // STACK
  uint16_t stack[MAX_NESTING_LEVEL];

  // TIMER
  uint8_t delay_timer;      // count at 60Hz, until reaches 0
  uint8_t sound_timer;      // count at 60Hz, until reaches 0. used for sound effects
                            // makes beeping sound, when value is non-zero
  
  // INPUT (keyboard)
  bool keypad[KEYS_COUNT];

  // LOADED ROM (place where ROM is stored)
  char *rom;                 // currently running ROM
} emulator_t;



/* ================ FUNCTION DECLERATIONS ============== */


#endif   // __CHIP8_HEADER_FILE__