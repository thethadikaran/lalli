#include "chip8.h"


bool sdl_init(sdlc_t *sdlc, const config_t *config) {
  /* I. initialize SDL subsystems */
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
    SDL_Log("Failed to initialized SDL2 subsystems!!! - %s\n", SDL_GetError());
    return false;
  }

  /* II. initialize SDL window */
  sdlc->win = SDL_CreateWindow(
                "CHIP8 Emulator",                  // window title
                SDL_WINDOWPOS_CENTERED,            // x positon of window
                SDL_WINDOWPOS_CENTERED,            // y position of window
                config->win_width * SCALE_FACTOR,  // width of window
                config->win_height * SCALE_FACTOR, // height of window
                0                                  // flags
              );
  if (!sdlc->win) {
    SDL_Log("Cannot initialize the Window - %s\n", SDL_GetError());
    return false;
  }

  /* III. initialize a renderer */
  sdlc->render = SDL_CreateRenderer(
                    sdlc->win,                 // window to display rendering
                    -1,                        // index of rendering driver (-1 to initilize with first one)
                    SDL_RENDERER_ACCELERATED   // flag
                );
  if (!sdlc->render) {
    SDL_Log("Cannot initialize the Renderer - %s\n", SDL_GetError());
    return false;
  }

  return true;    // initialization was successful
}



bool set_config_from_args(config_t *config, int argc, char **argv) {
  /* default configurations */
  config->win_height = CHIP8_NATIVE_HEIGHT;   
  config->win_width  = CHIP8_NATIVE_WIDTH;

  config->r = 0;    // inital background color to green
  config->g = 255;
  config->b = 0;
  config->a = 0;

  /* override defaults based on CLI arguments */
  (void)argc;  // prevent compiler error for unused variables
  (void)argv;


  return true;
}



void clear_screen(const sdlc_t *sdlc, const config_t *conf) {
  // background screen color (yellow)
  SDL_SetRenderDrawColor(sdlc->render, conf->r, conf->g, conf->b, conf->a);

  // clear the rendering by drawing background color
  SDL_RenderClear(sdlc->render);
}



void update_screen(const sdlc_t *sdlc) {
  // after composing the entire scene/drawing in the backbuffer then
  // call this function once per frame to present the backbuffer to the screen
  SDL_RenderPresent(sdlc->render);
}



void handle_inputs(emulator_t *emulator) {
  SDL_Event event;

  // pollevent removes the event from the queue and place it in the type member
  // of the event variable (above). if there are no event, then 0 is returned
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:           // exit the emulator
        emulator->status = STOP;
        return;
      
      case SDL_KEYDOWN:       // keyboard key is pressed
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:   // ESC key is pressed
            emulator->status = STOP;
            return;

          default:
            break;
        }
        break;
      
      case SDL_KEYUP:         // keyboard key is released
        break;

      default:
        break;
    }
  }
}



void sdl_free(sdlc_t *sdlc) {
  SDL_DestroyRenderer(sdlc->render); // destroy the renderer
  SDL_DestroyWindow(sdlc->win);      // destroy the SDL Window
  SDL_Quit();                        // shutdown all SDL subsystems
}


bool emulator_init(emulator_t *emulator) {
  // load font

  // load ROM

  // set chip8 emulator defaults
  emulator->status = RUNNING;    // turn on the emulator

  return true;
}







/* All starts here */
int main(int argc, char *argv[]) {
  // I. initialize emulator configurations/settings from the command line arguments
  config_t config = {0};
  if (!set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);

  // II. initialize the SDL subsystems & create window
  sdlc_t sdlc = {0};
  if (!sdl_init(&sdlc, &config)) exit(EXIT_FAILURE);

  // III. initialize the emulator machine
  emulator_t emulator = {0};
  if (!emulator_init(&emulator)) exit(EXIT_FAILURE);

  // IV. clear the screen
  clear_screen(&sdlc, &config);

  // V. emulator loop
  while (emulator.status != STOP) {
    // handle user inputs (events)
    handle_inputs(&emulator);
  
    // delay (60Hz or 60FPS)
    SDL_Delay(16);       // delay in milliseconds

    // update the window with the changes
    update_screen(&sdlc);
    
  }

  // VI. cleanup SDL subsystems
  sdl_free(&sdlc);

  return EXIT_SUCCESS;
}