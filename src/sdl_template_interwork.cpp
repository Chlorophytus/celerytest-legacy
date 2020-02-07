#include "../include/sdl_template_interwork.hpp"
using namespace sdl_template;

interwork::interwork(U16 _w, U16 _h) : w{_w}, h{_h} {
  // SDL will also return a non-zero value if we can't allocate a window nor a
  // renderer.
  assert(SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer) == 0);
}

bool interwork::tick() {
  SDL_Event event;
  SDL_PollEvent(&event);
  // If true, we keep ticking. If false, quit.
  return event.type != SDL_QUIT;
}

interwork::~interwork() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}