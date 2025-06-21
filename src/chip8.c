#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"          // for showing window

#define CHIP8_NATIVE_WIDTH    64    // 32x64 is the native CHIP8 emulator reolution
#define CHIP8_NATIVE_HEIGHT   32

/* struct to hold the emulator configurations */
typedef struct {
  uint32_t win_width;      // SDL window width
  uint32_t win_height;     // SDL window height
} config_t;

/* struct to hold the SDL components */
typedef struct {
  SDL_Window *win;       // main SDL window pointer
} sdlc_t;






bool sdl_init(sdlc_t *sdlc, const config_t *config) {
  // I. initialize SDL subsystems
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
    SDL_Log("Failed to initialized SDL2 subsystems!!! - %s\n", SDL_GetError());
    return false;
  }

  // II. initialize SDL window
  sdlc->win = SDL_CreateWindow(
                "CHIP8 Emulator",            // window title
                SDL_WINDOWPOS_CENTERED,      // x positon of window
                SDL_WINDOWPOS_CENTERED,      // y position of window
                config->win_width,           // width of window
                config->win_height,          // height of window
                0                            // flags
             );
  if (!sdlc->win) {
    SDL_Log("Cannot initialize the Window - %s\n", SDL_GetError());
    return false;
  }

  return true;    // initialization was successful
}



bool set_config_from_args(config_t *config, int argc, char **argv) {
  // default configurations
  config->win_height = CHIP8_NATIVE_HEIGHT;   
  config->win_width  = CHIP8_NATIVE_WIDTH;

  // override defaults based on CLI arguments
  (void)argc;  // prevent compiler error for unused variables
  (void)argv;


  return true;
}



void sdl_free(sdlc_t *sdlc) {
  SDL_DestroyWindow(sdlc->win);   // destroy the SDL Window
  SDL_Quit();                    // shutdown all SDL subsystems
}




/* All starts here */
int main(int argc, char *argv[]) {
  // I.initialize emulator configurations/settings from the command line arguments
  config_t config = {0};
  if (!set_config_from_args(&config, argc, argv)) return EXIT_FAILURE;

  // II.initialize the SDL subsystems & create window
  sdlc_t sdlc = {0};
  if (!sdl_init(&sdlc, &config)) return EXIT_FAILURE;

  SDL_Delay(3000);


  // cleanup SDL subsystems
  sdl_free(&sdlc);

  return EXIT_SUCCESS;
}