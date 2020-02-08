#include "../include/celerytest_interwork.hpp"
using namespace celerytest;

interwork::interwork(U16 _w, U16 _h) : w{_w}, h{_h} {
  // SDL will also return a non-zero value if we can't allocate a window.4
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  window = SDL_CreateWindow("celerytest", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
  assert(window);
  ctx = SDL_GL_CreateContext(window);
  assert(ctx);
  glewExperimental = GL_TRUE;
  auto e = glewInit();
  if(e != GLEW_OK) {
    std::fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(e));
  }
  assert(e == GLEW_OK);

  lua_ctx = std::make_unique<lua>();
}

bool interwork::tick() {
  SDL_Event event;
  SDL_PollEvent(&event);
  // If true, we keep ticking. If false, quit.
  return event.type != SDL_QUIT;
}

interwork::~interwork() {
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
}