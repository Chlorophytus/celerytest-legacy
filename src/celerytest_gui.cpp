#include "../include/celerytest_gui.hpp"

using namespace celerytest;

void gui::ctrl::blit_here(SDL_Surface *destination) const {
  SDL_BlitSurface(surface, nullptr, destination, rect.get());
}
// You must have the details (size/text/color/path) defined
void gui::text_ctrl::post_create() {
  object::post_create();
  con::log_all(con::severity::debug, {"post create guitextctrl"});

  font = TTF_OpenFont(font_path.c_str(), font_size);
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
  object::pre_destroy();
}
