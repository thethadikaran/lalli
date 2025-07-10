#include "chip8.h"


bool sdl_init(sdlc_t *sdlc, const config_t *config) {
  /* I. initialize SDL subsystems along with audio and video */
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
  // validate input parameter
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <rom_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* default configurations */
  config->win_height = SCREEN_HEIGHT;   
  config->win_width  = SCREEN_WIDTH;

  config->r = 0;    // inital background color to black
  config->b = 0;
  config->g = 0;
  config->a = 0;

  // udpate the rom file path
  config->rom = argv[1];

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
          case SDLK_ESCAPE:               // ESC key is pressed
            emulator->status = STOP;      // quit the emulator
            return;

          case SDLK_SPACE:                // SPACE key is pressed
            // toggle the emulator states
            emulator->status = emulator->status == RUNNING ? PAUSED : RUNNING;

            if (emulator->status == PAUSED) puts("=== ENTERING DEBUG MODE ===");
            else puts("=== EXIT DEBUG MODE ===");
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



bool emulator_init(emulator_t *emulator, char *rom) {
  const uint32_t entry_point = 0x200;    // RAM, from 0x00 (0) -> 0x1FF (511) is
                                         // occupied by the emulator itself.
                                         // reading of instructions starts from 
                                         // the next memory location 0x200 (512)

  // clear RAM, stack, display & registors
  memset(emulator->ram, 0, EMULATOR_RAM * sizeof(uint8_t));
  memset(emulator->stack, 0, STACK_SIZE * sizeof(uint16_t));
  memset(emulator->display, 0, SCREEN_HEIGHT * SCREEN_WIDTH);
  memset(emulator->V, 0, DATA_REGISTER * sizeof(uint8_t));

  // set chip8 emulator defaults
  emulator->status = RUNNING;    // turn on the emulator
  emulator->PC = entry_point;    // start program counter at the RAM's entry point
  emulator->rom = rom;           // update the ROM file name

  // load fontset in to RAM (at the starting location of RAM)
  memcpy(emulator->ram, fontset, sizeof(fontset));

  // open the ROM and load the contents into the RAM
  if (!load_rom(emulator)) return false;

  return true;
}



/*** UTIL ****/
bool load_rom(emulator_t *emulator) {
  // open the ROM
  FILE *rom = fopen(emulator->rom, "rb");      // reading the binary ROM file
  if (!rom) {
    SDL_Log("ROM file \"%s\" is invalid or does not exist", emulator->rom);
    return false;
  }

  // calcuate the size of the loaded rom
  size_t rsize = rom_size(rom);

  // verify if the rom can fit in RAM
  if (rsize > (sizeof(emulator->ram) - emulator->PC) ) {
    SDL_Log("Loaded ROM can't fit in to the RAM");
    fclose(rom);
    return false;
  }

  // load the ROM to RAM at the entry point
  bool status = fread(&emulator->ram[emulator->PC], rsize, 1, rom);
  if (!status)  {
    SDL_Log("Cannot load ROM on to the RAM");
    fclose(rom);
    return false;
  }

  // finally close the file
  fclose(rom);
  return true;
}



size_t rom_size(FILE *rom) {
  // move the file pointer to the end of the file
  fseek(rom, 0, SEEK_END);

  // tis position represent the offset in bytes from begining of the file
  size_t size = ftell(rom);

  // reset the function pointer to the beginning of the file
  rewind(rom);

  return size;
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
  if (!emulator_init(&emulator, config.rom)) exit(EXIT_FAILURE);

  // IV. clear the screen
  clear_screen(&sdlc, &config);

  // V. emulator loop
  while (emulator.status != STOP) {
    // handle user inputs (events)
    handle_inputs(&emulator);

    // entering debug mode
    if (emulator.status == PAUSED) continue;

    // emulate the chip8 instruction
  
    // delay (60Hz or 60FPS)
    SDL_Delay(16);                 // delay in milliseconds

    // update the window with the changes
    update_screen(&sdlc);
  }

  // VI. cleanup SDL subsystems
  sdl_free(&sdlc);

  return EXIT_SUCCESS;
}