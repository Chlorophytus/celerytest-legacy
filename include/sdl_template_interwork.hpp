#pragma once
#include "sdl_template.hpp"

namespace sdl_template {
struct interwork {
  SDL_Window *window;
  SDL_Renderer *renderer;
  U16 w, h;
  bool tick();
  interwork(U16, U16);
  ~interwork();
};
} // namespace sdl_template
