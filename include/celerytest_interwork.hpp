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

  // OpenGL forces you to draw tris even with 2D
  GLuint ebo2d, vao2d, vbo2d;

  bool fullscreen;
  bool tick();
  interwork(U16, U16, bool);
  ~interwork();
};
} // namespace celerytest
