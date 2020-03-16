#include "../include/celerytest_interwork.hpp"
#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_sim.hpp"
using namespace celerytest;

void celerytest::check_sdl_error() {
  log(severity::error, {"SDL2 error: ", SDL_GetError()});
}

interwork::interwork(U16 _w, U16 _h, bool _fullscreen)
    : w{_w}, h{_h}, fullscreen{_fullscreen}, framebuffer{new U32[_w * _h]}{
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
#if 0
  F32 verts[8]{
      -0.5f, -0.5f, // 0
      -0.5f, 0.5f,  // 1
      0.5f,  0.5f,  // 2
      0.5f,  -0.5f  // 3
  };
  U32 elems[6]{0, 1, 2, 2, 3, 0};
  glGenVertexArrays(1, &vao_test);
  glBindVertexArray(vao_test);
  glGenBuffers(1, &ebo_test);
  glGenBuffers(1, &vbo_test);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_test);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_test);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);
  glEnableVertexAttribArray(10);
  glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(F32), 0x0);
#endif
  surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);
  glGenTextures(1, &tex2d);
  glGenFramebuffers(1, &fbo2d);
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  for (auto &&ui_elemid : *get_env2d()) {
    auto &&ui_elem = celerytest::sim_reference(ui_elemid);
    assert(ui_elem->get_type() == celerytest::sim_types::env2duiobject);
    auto &&casted = dynamic_cast<celerytest::env2d_uiobject *>(ui_elem);
    casted->tick();
    if (!casted->show) {
      continue;
    }
    SDL_Rect r{.x = casted->x, .y = casted->y, .w = casted->w, .h = casted->h};
    SDL_BlitSurface(casted->surf, nullptr, surf, &r);
  }
  // bind...
  glBindTexture(GL_TEXTURE_2D, tex2d);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo2d);
  SDL_LockSurface(surf);
  auto pixels = reinterpret_cast<U8 *>(surf->pixels);
  U8 sr, sg, sb, sa;
  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      auto index = i * surf->pitch + (j << 2);
      SDL_GetRGBA(*reinterpret_cast<U32 *>(pixels + index), surf->format, &sr,
                  &sg, &sb, &sa);
      framebuffer[i * w + j] = (U32(sa) << 0x18) | (U32(sb) << 0x10) |
                               (U32(sg) << 0x08) | (U32(sr) << 0x00);
    }
  }
  SDL_UnlockSurface(surf);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               framebuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         tex2d, 0);
  // finalize
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo2d);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, w, h, 0, h, w, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
  // glDisableVertexAttribArray(10);
  delete[] framebuffer;
  glDeleteFramebuffers(1, &fbo2d);
  glDeleteTextures(1, &tex2d);
  SDL_FreeSurface(surf);
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
}