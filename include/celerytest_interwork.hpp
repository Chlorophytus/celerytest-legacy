#pragma once
#include "celerytest.hpp"
#include "celerytest_lua.hpp"

namespace celerytest {
  void check_sdl_error();
struct interwork {
  SDL_Window *window;
  SDL_GLContext ctx;
  SDL_Surface *surf;
  std::unique_ptr<lua> lua_ctx;
  U16 w, h;

  GLuint ebo_test, vao_test, vbo_test;
  // OpenGL ALSO forces you to handle blitting weirdly
  GLuint fbo2d, tex2d;
  U32 *framebuffer;
  bool fullscreen;
  bool tick();
  interwork(U16, U16, bool);
  ~interwork();
};
} // namespace celerytest
