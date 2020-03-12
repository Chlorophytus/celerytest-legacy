#include "../include/celerytest_interwork.hpp"
#include "../include/celerytest_log.hpp"
using namespace celerytest;

void celerytest::check_sdl_error() {
  log(severity::error, {"SDL2 error: ", reinterpret_cast<const char *>(SDL_GetError())});
}

interwork::interwork(U16 _w, U16 _h, bool _fullscreen)
    : w{_w}, h{_h}, fullscreen{_fullscreen} {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  // SDL will also return a non-zero value if we can't allocate a window.
  if (fullscreen) {
    window = SDL_CreateWindow("celerytest", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
  } else {
    window = SDL_CreateWindow("celerytest", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
  }
  if(!window) {
    check_sdl_error();
  }
  assert(window);
  ctx = SDL_GL_CreateContext(window);
  if(!ctx) {
    check_sdl_error();
  }
  assert(ctx);
  glewExperimental = GL_TRUE;
  auto e = glewInit();
  if (e != GLEW_OK) {
    log(severity::error, {"GLEW error: ", reinterpret_cast<const char *>(glewGetErrorString(e))});
  }
  assert(e == GLEW_OK);

  lua_ctx = std::make_unique<lua>();
  lua_ctx->load("foo");
}

bool interwork::tick() {
  SDL_Event event;
  SDL_PollEvent(&event);

  // TODO: Insert some OpenGL calls here
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  SDL_GL_SwapWindow(window);

  // If true, we keep ticking. If false, quit.
  if(event.type == SDL_KEYUP) {
    if(event.key.keysym.sym == SDLK_ESCAPE) {
      return false;
    }
  }
  return event.type != SDL_QUIT;
}

interwork::~interwork() {
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
}