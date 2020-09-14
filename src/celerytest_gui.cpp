#include "../include/celerytest_gui.hpp"

using namespace celerytest;

// === GUI Controls ===========================================================
void gui::ctrl::blit_here(SDL_Surface *destination) const {
  if (filter.has_value()) {
    SDL_BlitSurface(filtered, nullptr, destination, rect.get());
  } else {
    SDL_BlitSurface(surface, nullptr, destination, rect.get());
  }
}
void gui::ctrl::post_create() {
  object::post_create();
  con::log_all(con::severity::debug, {"post create guictrl"});

  if (filter.has_value()) {
    glGenTextures(2, filter->textures);

    filter->shader_object = glCreateShader(GL_COMPUTE_SHADER);

    auto src = new char[filter->source.size()]{0};
    strcpy(src, filter->source.data());

    glShaderSource(filter->shader_object, 1, &src, nullptr);
    con::log_all(con::severity::debug, {"compiling GUI shader"});
    glCompileShader(filter->shader_object);

    char shader_log[4096]{0};
    glGetShaderInfoLog(filter->shader_object, 4096, nullptr, shader_log);
    con::log_all(con::severity::debug,  std::string{"COMPILE:\n"} + shader_log);

    filter->shader_program = glCreateProgram();
    glAttachShader(filter->shader_program, filter->shader_object);
    con::log_all(con::severity::debug, {"linking GUI shader"});
    glLinkProgram(filter->shader_program);

    char program_log[4096]{0};
    glGetProgramInfoLog(filter->shader_object, 4096, nullptr, program_log);
    con::log_all(con::severity::debug,  std::string{"LINK:\n"} + program_log);

    delete[] src;

    filtered = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0xFF000000,
                                    0x00FF0000, 0x0000FF00, 0x000000FF);
  }
}
void gui::ctrl::do_filter() {
  glBindTexture(GL_TEXTURE_2D, filter->textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, GL_RGBA,
               GL_UNSIGNED_INT_8_8_8_8, surface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glBindTexture(GL_TEXTURE_2D, filter->textures[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, GL_RGBA,
               GL_UNSIGNED_INT_8_8_8_8, filtered->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glUseProgram(filter->shader_program);
  glBindImageTexture(0, filter->textures[0], 0, false, 0, GL_READ_ONLY,
                     GL_RGBA32F);
  glBindImageTexture(1, filter->textures[1], 0, false, 0, GL_WRITE_ONLY,
                     GL_RGBA32F);

  for (auto &&kv : filter->uniforms) {
    if (kv.first < 2) {
      throw std::runtime_error{"GuiCtrl filter uniforms below 2 are reserved"};
    }
    switch (kv.second.type) {
    case uniform_types::float4:
      glUniform4f(kv.first, kv.second.data.i[0], kv.second.data.f[1],
                  kv.second.data.f[2], kv.second.data.f[3]);
      break;
    case uniform_types::int4:
      glUniform4i(kv.first, kv.second.data.f[0], kv.second.data.i[1],
                  kv.second.data.i[2], kv.second.data.i[3]);
      break;
    case uniform_types::uint4:
      glUniform4ui(kv.first, kv.second.data.u[0], kv.second.data.u[1],
                   kv.second.data.u[2], kv.second.data.u[3]);
      break;
    }
  }

  glDispatchCompute(filtered->w, filtered->h, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
void gui::ctrl::pre_destroy() {
  con::log_all(con::severity::debug, {"pre destroy guictrl"});
  if (filter.has_value()) {
    glDetachShader(filter->shader_program, filter->shader_object);
    glDeleteShader(filter->shader_object);
    glDeleteProgram(filter->shader_program);
    glDeleteTextures(2, filter->textures);
  }
  object::pre_destroy();
}

// You must have the details (size/text/color/path) defined
void gui::text_ctrl::post_create() {
  ctrl::post_create();
  con::log_all(con::severity::debug, {"post create guitextctrl"});

  font = TTF_OpenFont(font_path.string().c_str(), font_size);
  dirty = true;
}
void gui::text_ctrl::on_paint() {
  // putting tracers here to debug double allocs/frees when we get them
  con::log_all(con::severity::debug, {"painting a text surface"});
  if (dirty) {
    if (free_this) {
      con::log_all(con::severity::debug, {"freeing a text surface"});
      SDL_FreeSurface(surface);
      free_this = false;
    }
    con::log_all(con::severity::debug, {"allocating a text surface"});
    surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    dirty = false;
    free_this = true;
  }
}
void gui::text_ctrl::pre_destroy() {
  con::log_all(con::severity::debug, {"pre destroy guitextctrl"});
  if (free_this) {
    con::log_all(con::severity::debug, {"freeing a text surface 2"});
    SDL_FreeSurface(surface);
    free_this = false;
  }
  TTF_CloseFont(font);
  ctrl::pre_destroy();
}
