#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_log.hpp"
using namespace celerytest;

env2d_uiobject::env2d_uiobject()
    : resize{true}, dirty_before{false}, empty_before{true} {}

void env2d_uiobject::fill(U16 _w, U16 _h, U16 _x, U16 _y) {
  assert(empty_before);
  empty_before = false;
  w = _w;
  h = _h;
  x = _x;
  y = _y;
  framebuffer = new U32[_w * _h];
  for (auto i = 0; i < _h; i++) {
    for (auto j = 0; j < _w; j++) {
      framebuffer[i * _w + j] = 0xFFFFFFFF;
    }
  }
}

void env2d_uiobject::tick() {}

env2d_uiobject::~env2d_uiobject() {
  if (framebuffer != nullptr) {
    delete[] framebuffer;
  }
}

env2d_conobject::env2d_conobject() : env2d_uiobject(), dirty(true) {
  // TODO: dynamic fonts
  font = TTF_OpenFont("/usr/share/fonts/gnu-free/FreeMono.otf", 14);
  surf =
      TTF_RenderText_Blended(font, "There's a new kid in town...",
                             SDL_Color{0, 255, 0, 255});
  if (surf == nullptr) {
    log(severity::error, {"TTF: ", TTF_GetError()});
  }
  assert(surf != nullptr);
}

void env2d_conobject::tick() {
  if (!dirty) {
    return;
  }
  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      framebuffer[i * w + j] = 0xFF202020;
    }
  }

  // very sloppy math ahead
  SDL_LockSurface(surf);
  auto pixels = reinterpret_cast<U8 *>(surf->pixels);
  U8 r, g, b, a;
  for (auto i = 0; i < surf->h; i++) {
    for (auto j = 0; j < surf->w; j++) {
      auto index = i * surf->pitch + (j << 2);
      SDL_GetRGBA(*reinterpret_cast<U32 *>(pixels + index), surf->format, &r,
                  &g, &b, &a);
      if (a) {
        framebuffer[i * w + j] =
            (U32(a) << 24) | (U32(r) << 16) | (U32(g) << 8) | (U32(b) << 0);
        framebuffer[(i + 4) * w + (j + 4)] = 0xFF000000;
      }
    }
  }
  SDL_UnlockSurface(surf);
}

env2d_conobject::~env2d_conobject() { SDL_FreeSurface(surf); }