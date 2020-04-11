#include "../include/celerytest_interwork.hpp"
#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_sim.hpp"
using namespace celerytest;

void celerytest::check_sdl_error() {
  log(severity::error, {"SDL2 error: ", SDL_GetError()});
}

interwork::interwork(U16 _w, U16 _h, bool _fullscreen)
    : w{_w}, h{_h}, fullscreen{_fullscreen}, framebuffer{new U32[_w * _h]},
      ticks{0}, t0{std::chrono::high_resolution_clock::now()} {
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

  glGenVertexArrays(1, &vao3d);
  glBindVertexArray(vao3d);
  glGenBuffers(1, &vbo3d);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(F32), 0x0);
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
  surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ABGR8888);
  glGenTextures(1, &tex2d);
  glGenFramebuffers(1, &fbo2d);
  lua_ctx = std::make_unique<lua>();
  lua_ctx->load("init.lua");

  get_con2d();
  get_mission(mission_types::null);
  emplace_logger(&con2d_log);
  log(severity::warn, {"2D console hooked."});
}

bool interwork::tick() {

  // TODO: Insert some OpenGL calls here
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  bool dirty = false;
  assert(celerytest::get_missiontype() != nullptr);
  auto mtype = *celerytest::get_missiontype();
  auto &&rawmission = celerytest::sim_reference(celerytest::get_mission(mtype));
  auto &&mission = dynamic_cast<celerytest::mission_null *>(rawmission);
  if (mission->tick()) {
    auto num_elems_in_mission = mission->get_terrain_pitch();
    num_elems_in_mission *= num_elems_in_mission;
    if (num_elems_in_mission > 0) {
      auto verts = mission->get_terrain_data();
      auto raw_vertex_data = std::vector<F32>{};
      for (auto &&vert : verts) {
        raw_vertex_data.emplace_back(vert.x);
        raw_vertex_data.emplace_back(vert.y);
        raw_vertex_data.emplace_back(vert.z);
        raw_vertex_data.emplace_back(vert.w);
      }
      glBindBuffer(GL_ARRAY_BUFFER, vbo3d);
      glBufferData(GL_ARRAY_BUFFER, sizeof(F32) * raw_vertex_data.size(),
                   raw_vertex_data.data(), GL_STATIC_DRAW);
      glDrawElements(GL_TRIANGLES, num_elems_in_mission * 2, GL_UNSIGNED_INT,
                     0);
    }
  }
  //
  for (auto &&ui_elemid : *get_env2d()) {
    auto &&ui_elem = celerytest::sim_reference(ui_elemid);
    assert(ui_elem->get_type() == celerytest::sim_types::env2duiobject);
    auto &&casted = dynamic_cast<celerytest::env2d_uiobject *>(ui_elem);

    casted->tick();
    if (!casted->show) {
      continue;
    }
    SDL_Rect r{.x = casted->x, .y = casted->y, .w = casted->w, .h = casted->h};
    if (casted->dirty) {
      SDL_BlitSurface(casted->surf, nullptr, surf, &r);
      dirty = true;
    }
  }
  // bind...
  glBindTexture(GL_TEXTURE_2D, tex2d);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo2d);
  auto pixels = reinterpret_cast<U8 *>(surf->pixels);
  if (dirty) {
    SDL_LockSurface(surf);
    for (auto i = 0; i < h; i++) {
      for (auto j = 0; j < w; j++) {
        auto index = i * surf->pitch + (j << 2);
        // HA!
        framebuffer[i * w + j] = *reinterpret_cast<U32 *>(pixels + index);
      }
    }
    SDL_UnlockSurface(surf);
    dirty = false;
  }
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

  ticks++;
  auto t1 = std::chrono::high_resolution_clock::now();
  auto time =
      std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  if (time > 1000) {
    log(severity::info, {std::to_string(ticks), "fps"});
    ticks = 0;
    t0 = std::chrono::high_resolution_clock::now();
  }

  SDL_GL_SwapWindow(window);

  // If true, we keep ticking. If false, quit.
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_TEXTINPUT:
  case SDL_KEYDOWN:
  case SDL_KEYUP: {
    auto &&con = dynamic_cast<env2d_conobject *>(sim_reference(get_con2d()));
    auto cast_key = event.key;
    if (event.type == SDL_KEYUP) {
      switch (cast_key.keysym.sym) {
      case '`': {
        con->curr_prompt.pop_back();
        if (con->show) {
          con->show = false;
        } else {
          con->show = true;
        }
        return true;
      }
      case SDLK_BACKSPACE: {
        if (con->curr_prompt.empty())
          return true;
        con->curr_prompt.pop_back();
        return true;
      }
      case SDLK_ESCAPE: {
        return false;
      }
      case SDLK_RETURN: {
        if (!con->curr_prompt.empty()) {
          auto err = luaL_dostring(lua_ctx->L, con->curr_prompt.c_str());
          log(severity::info, {"lua> ", con->curr_prompt});
          if (err) {
            auto str = lua_tostring(lua_ctx->L, -1);
            log(severity::error, {str});
            lua_pop(lua_ctx->L, -1);
          }
          con->curr_prompt = std::string{" "};
          con->dirty = true;
        }
        return true;
      }
      default: {
        if (!con->show) {
          lua_ctx->kmaps_call(lua_ctx->L, cast_key.keysym.sym,
                              cast_key.type == SDL_KEYDOWN);
          return true;
        }
      }
      }
    }
    if (event.type == SDL_TEXTINPUT) {
      auto cast_text = event.text;
      con->curr_prompt += cast_text.text;
    }
    return true;
  }
  case SDL_QUIT: {
    return false;
  }
  default: {
    return true;
  }
  }
  assert(false);
}

interwork::~interwork() {
  glDisableVertexAttribArray(0);
  delete[] framebuffer;
  glDeleteFramebuffers(1, &fbo2d);
  glDeleteTextures(1, &tex2d);
  SDL_FreeSurface(surf);
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
}