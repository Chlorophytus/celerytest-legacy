#include "../include/celerytest_con.hpp"

#include "../include/celerytest_gl.hpp"

using namespace celerytest;

static bool context_present = false;
static SDL_Window *sdl_window;
static SDL_GLContext sdl_gl_context;
static U16 width;
static U16 height;

void gl::create_context(std::string_view &title, U16 &&w, U16 &&h, bool &&f) {
  if (context_present) {
    throw std::runtime_error{"only one celerytest GL context can be allocated"};
  }

  con::log_all(con::severity::informational, {"creating an OpenGL context"});

  width = w;
  height = h;
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  sdl_window = SDL_CreateWindow(
      title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
      height, SDL_WINDOW_OPENGL | (f ? SDL_WINDOW_FULLSCREEN : 0));
  sdl_gl_context = SDL_GL_CreateContext(sdl_window);

  glewExperimental = GL_TRUE;
  glewInit();
  context_present = true;
}

[[maybe_unused]] bool gl::tick() {
  return true;
}

void gl::remove_context() {
  if (!context_present) {
    throw std::runtime_error{"celerytest GL context was double-freed"};
  }

  con::log_all(con::severity::informational, {"removing an OpenGL context"});
  SDL_GL_DeleteContext(sdl_gl_context);

  context_present = false;
}