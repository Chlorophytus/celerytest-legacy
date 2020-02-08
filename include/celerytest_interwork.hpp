#pragma once
#include "celerytest.hpp"
#include "celerytest_lua.hpp"

namespace celerytest {
  void check_sdl_error();
struct interwork {
  SDL_Window *window;
  SDL_GLContext ctx;
  std::unique_ptr<lua> lua_ctx;
  U16 w, h;
  bool fullscreen;
  bool tick();
  interwork(U16, U16, bool);
  ~interwork();
};
} // namespace celerytest
