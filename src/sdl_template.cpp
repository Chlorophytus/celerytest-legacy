#include "../include/sdl_template.hpp"
#include "../include/sdl_template_interwork.hpp"

int main(int argc, char **argv) {
  std::printf("sdl2 template %s\n", sdl_template_VSTRING_FULL);
  // SDL will return a non-zero value for failure in initialization.
  assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
  auto inter = std::make_unique<sdl_template::interwork>(640, 480);

  while(inter->tick())
    SDL_Delay(10);

  // Quit SDL lastly, **LASTLY**.
  SDL_Quit();
  return 0;
}