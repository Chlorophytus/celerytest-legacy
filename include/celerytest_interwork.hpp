#pragma once
#include "celerytest.hpp"
#include "celerytest_lua.hpp"

namespace celerytest {
struct interwork {
  SDL_Window *window;
  SDL_GLContext ctx;
  std::unique_ptr<lua> lua_ctx;
  U16 w, h;
  bool tick();
  interwork(U16, U16);
  ~interwork();
};
} // namespace celerytest
