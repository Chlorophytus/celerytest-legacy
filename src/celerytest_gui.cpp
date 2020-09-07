#include "../include/celerytest_gui.hpp"

using namespace celerytest;

// === GUI Controls ===========================================================
void gui::ctrl::blit_here(SDL_Surface *destination) const {
  if (shader.has_value()) {
    SDL_BlitSurface(shaded, nullptr, destination, rect.get());
  } else {
    SDL_BlitSurface(surface, nullptr, destination, rect.get());
  }
}
// You must have the details (size/text/color/path) defined
void gui::text_ctrl::post_create() {
  object::post_create();
  con::log_all(con::severity::debug, {"post create guitextctrl"});

  font = TTF_OpenFont(font_path.c_str(), font_size);
  dirty = true;
  if (shader.has_value()) {
    do_shade = true;
  }
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

  if (do_shade) {
    do_shade = false;
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
  object::pre_destroy();
}

// === GUI Shaders ============================================================
#if 0
void gui::shader2d::post_create() {
  object::post_create();
  con::log_all(con::severity::debug, {"post create guishader2d"});
  shader_object = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(shader_object, 1, (const char *const[]){source.data()},
                 nullptr);
  con::log_all(con::severity::debug, {"compiling GUI shader"});
  glCompileShader(shader_object);

  char log[4096]{0};
  glGetShaderInfoLog(shader_object, 4096, nullptr, log);
  con::log_all(con::severity::debug, {"LOG: ", log});

  shader_program = glCreateProgram();
  glAttachShader(shader_program, shader_object);
  con::log_all(con::severity::debug, {"linking GUI shader"});
  glLinkProgram(shader_program);
}
void gui::shader2d::pre_destroy() {
  con::log_all(con::severity::debug, {"pre destroy guishader2d"});
  glDetachShader(shader_program, shader_object);
  glDeleteShader(shader_object);
  glDeleteProgram(shader_program);
  object::pre_destroy();
}
void gui::shader2d::shade_here(SDL_Surface *dst) const {
  glUseProgram(shader_program);
}
#endif