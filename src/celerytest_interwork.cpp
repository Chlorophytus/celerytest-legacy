#include "../include/celerytest_interwork.hpp"
#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_sim.hpp"
using namespace celerytest;

void celerytest::check_sdl_error() {
  log(severity::error, {"SDL2 error: ", SDL_GetError()});
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
  if (!window) {
    check_sdl_error();
  }
  assert(window);
  ctx = SDL_GL_CreateContext(window);
  if (!ctx) {
    check_sdl_error();
  }
  assert(ctx);
  glewExperimental = GL_TRUE;
  auto e = glewInit();
  if (e != GLEW_OK) {
    log(severity::error, {"GLEW error: ", reinterpret_cast<const char *>(
                                              glewGetErrorString(e))});
  }
  assert(e == GLEW_OK);
  F32 verts[8]{
      -1.0f, -1.0f, // 0
      -1.0f, 1.0f,  // 1
      1.0f,  1.0f,  // 2
      1.0f,  -1.0f  // 3
  };
  U32 elems[6]{0, 1, 2, 2, 3, 0};
  glGenVertexArrays(1, &vao2d);
  glBindVertexArray(vao2d);
  glGenBuffers(1, &ebo2d);
  glGenBuffers(1, &vbo2d);
  glBindBuffer(GL_ARRAY_BUFFER, vbo2d);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2d);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);
  glEnableVertexAttribArray(10);
  glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(F32), 0x0);

  lua_ctx = std::make_unique<lua>();
  lua_ctx->load("init.lua");

  get_con2d();
  emplace_logger(&con2d_log);
  log(severity::warn, {"2D console hooked."});
}

bool interwork::tick() {
  SDL_Event event;
  SDL_PollEvent(&event);

  // TODO: Insert some OpenGL calls here
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  for (auto &&ui_elemid : *get_env2d()) {
    auto &&ui_elem = celerytest::sim_reference(ui_elemid);
    assert(ui_elem->get_type() == celerytest::sim_types::env2duiobject);
    auto &&casted = dynamic_cast<celerytest::env2d_uiobject *>(ui_elem);
    assert(!casted->empty_before);
    casted->tick();
    if (casted->resize) {
      log(celerytest::severity::warn, {"resize"});
      if (casted->dirty_before) {
        log(celerytest::severity::warn, {"dirty before"});
        // k, I **THINK** we need to delete framebuffers/textures
        glDeleteFramebuffers(1, &casted->fbo);
        glDeleteTextures(1, &casted->tex);
        casted->dirty_before = false;
      }
      // then generate fbos/texs
      glGenTextures(1, &casted->tex);
      glGenFramebuffers(1, &casted->fbo);
      casted->resize = false;
    }
    // bind...
    glBindTexture(GL_TEXTURE_2D, casted->tex);
    glBindFramebuffer(GL_FRAMEBUFFER, casted->fbo);
    glBindTexture(GL_TEXTURE_2D, casted->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, casted->w, casted->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, casted->framebuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           casted->tex, 0);
    // finalize
    glBindFramebuffer(GL_READ_FRAMEBUFFER, casted->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, casted->w, casted->h, casted->x, h - casted->y,
                      casted->x + casted->w, h - casted->h + casted->y,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  SDL_GL_SwapWindow(window);

  // If true, we keep ticking. If false, quit.
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      return false;
    }
  }
  return event.type != SDL_QUIT;
}

interwork::~interwork() {
  glDisableVertexAttribArray(10);
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
}