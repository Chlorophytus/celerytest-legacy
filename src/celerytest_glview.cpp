#include "../include/celerytest_gl.hpp"

#include "../include/celerytest_glview.hpp"

using namespace celerytest;

void glview::view2d::post_create() {
  sim::object::post_create();
  con::log_all(con::severity::debug, {"post create glview2d"});

  surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000,
                                 0x0000FF00, 0x000000FF);
  auto debug_font = (font_dir / "SourceCodePro-Regular.ttf");
  con::log_all(con::severity::debug, {"with font ", debug_font});
  font = TTF_OpenFont(debug_font.c_str(), 16);

  glGenFramebuffers(1, &framebuffer);
  glGenTextures(1, &ui_texture);

  last_ms = 1000;
  render();
}
// will not delete the GuiCtrls for you!
void glview::view2d::pre_destroy() {
  con::log_all(con::severity::debug, {"pre destroy glview2d"});

  glDeleteFramebuffers(1, &framebuffer);
  glDeleteTextures(1, &ui_texture);

  SDL_FreeSurface(surface);

  sim::object::pre_destroy();
}
void glview::view2d::render() {
  t0 = SDL_GetTicks();
  con::log_all(con::severity::debug, {"pre render glview2d"});
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D, ui_texture);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  SDL_FillRect(surface, nullptr, 0x00000000);
  paint();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
               GL_UNSIGNED_INT_8_8_8_8, surface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         ui_texture, 0);
  glBlitNamedFramebuffer(framebuffer, 0, 0, h, w, 0, 0, 0, w, h,
                         GL_COLOR_BUFFER_BIT, GL_NEAREST);
  t1 = SDL_GetTicks();
  last_ms = t1 - t0;
  con::log_all(con::severity::debug, {"post render glview2d"});
}
// You can override this to change how the framebuffer is 2D painted by SDL2.
void glview::view2d::paint() {
  for (auto &&idx : ctrls_idx) {
    auto &&ptr = parent->query_object(idx);
    if (ptr->get_type() == sim::types::gui_ctrl ||
        ptr->get_type() == sim::types::gui_textctrl) {
      auto &&r_ptr = dynamic_cast<gui::ctrl *>(ptr);
      r_ptr->on_paint();
      r_ptr->blit_here(surface);
    }
  }
  auto font_surface = TTF_RenderUTF8_Blended(
      font,
      (last_ms > 0 ? std::string{"fps: "} + std::to_string(1000 / last_ms)
                   : std::string{"fps: 1000+"})
          .c_str(),
      (SDL_Color){.r = 0xFF, .g = 0xFF, .b = 0x80, .a = 0xFF});
  SDL_Rect rect{
      .x = 75, .y = h - 100, .w = font_surface->w, .h = font_surface->h};
  SDL_BlitSurface(font_surface, nullptr, surface, &rect);
  SDL_FreeSurface(font_surface);
}
