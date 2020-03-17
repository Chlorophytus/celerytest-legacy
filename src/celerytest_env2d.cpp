#include "../include/celerytest_env2d.hpp"
using namespace celerytest;

env2d_uiobject::env2d_uiobject() : empty_before(true), show(true) {}

void env2d_uiobject::fill(U16 _w, U16 _h, U16 _x, U16 _y, const char *path) {
  assert(empty_before);
  empty_before = false;
  x = _x;
  y = _y;
  surf = (path != nullptr) ? IMG_Load(path) : nullptr;
  if (surf) {
    w = surf->w;
    h = surf->h;
    return;
  } else {
    w = _w;
    h = _h;
    surf =
        SDL_CreateRGBSurfaceWithFormat(0, _w, _h, 32, SDL_PIXELFORMAT_RGBA8888);
  }
}

void env2d_uiobject::tick() {}

env2d_uiobject::~env2d_uiobject() {
  // if (framebuffer != nullptr) {
  //   delete[] framebuffer;
  // }
  SDL_FreeSurface(surf);
}

env2d_conobject::env2d_conobject() : env2d_uiobject(), playback{}, dirty(true) {
  // TODO: dynamic fonts
  font = TTF_OpenFont("/usr/share/fonts/gnu-free/FreeMono.otf", 15);
}

void env2d_conobject::tick() {
  if (!dirty)
    return;
  dirty = false;
  // for (auto i = 0; i < h; i++) {
  //   for (auto j = 0; j < w; j++) {
  //     framebuffer[i * w + j] = 0x10101010;
  //   }
  // }

  // very sloppy math ahead
  if (playback.empty()) {
    return;
  }
  U8 sr, sg, sb, sa;
  auto off = 0;
  auto size = S32(playback.size());
  SDL_FillRect(surf, nullptr, 0xFFFFFFDD);
  for (S32 i = size - 23; i < size; i++) {
    if (i < 0) {
      goto full;
    }
    if (!playback.at(i)->str.empty()) {
      switch (playback.at(i)->sev) {
      case severity::warn: {
        fontsurf = TTF_RenderText_Blended(font, playback.at(i)->str.c_str(),
                                          SDL_Color{127, 127, 127, 0xFF});
        break;
      }
      case severity::error: {
        fontsurf = TTF_RenderText_Blended(font, playback.at(i)->str.c_str(),
                                          SDL_Color{255, 0, 0, 0xFF});
        break;
      }
      default: {
        fontsurf = TTF_RenderText_Blended(font, playback.at(i)->str.c_str(),
                                          SDL_Color{0, 0, 0, 0xFF});
        break;
      }
      }
      if (fontsurf == nullptr) {
        log(severity::error, {"TTF: ", TTF_GetError()});
      }
      assert(fontsurf != nullptr);
      SDL_Rect r{.x = 0, .y = off, .w = fontsurf->w, .h = fontsurf->h};
      SDL_BlitSurface(fontsurf, nullptr, surf, &r);
      SDL_FreeSurface(fontsurf);
    }
  full:
    off += 15;
  }
  fontsurf = TTF_RenderText_Blended(font, "lua>_", SDL_Color{0, 0, 0, 0xFF});
  if (fontsurf == nullptr) {
    log(severity::error, {"TTF: ", TTF_GetError()});
  }
  assert(fontsurf != nullptr);
  SDL_Rect r{.x = 0, .y = off, .w = fontsurf->w, .h = fontsurf->h};
  SDL_BlitSurface(fontsurf, nullptr, surf, &r);
}