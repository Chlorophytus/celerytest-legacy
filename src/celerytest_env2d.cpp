#include "../include/celerytest_env2d.hpp"
using namespace celerytest;

U32 celerytest::calc_alpha2d(U32 dst, U32 src) {
  U8 src_r, src_g, src_b, src_a;
  U8 dst_r, dst_g, dst_b, dst_a;

  dst_b = (dst >> 0x00) & 0x000000FF;
  dst_g = (dst >> 0x08) & 0x000000FF;
  dst_r = (dst >> 0x10) & 0x000000FF;
  dst_a = (dst >> 0x18) & 0x000000FF;

  src_b = (src >> 0x00) & 0x000000FF;
  src_g = (src >> 0x08) & 0x000000FF;
  src_r = (src >> 0x10) & 0x000000FF;
  src_a = (src >> 0x18) & 0x000000FF;
  auto out_a = U32((src_a + (dst_a * (src_a)))) & 0x000000FF;
  auto out_r = U32(((src_r * src_a) + ((dst_r * dst_a) * (src_a))) /
                   (src_a + (dst_a * (src_a)))) &
               0x000000FF;
  auto out_g = U32(((src_g * src_a) + ((dst_g * dst_a) * (src_a))) /
                   (src_a + (dst_a * (src_a)))) &
               0x000000FF;
  auto out_b = U32(((src_b * src_a) + ((dst_b * dst_a) * (src_a))) /
                   (src_a + (dst_a * (src_a)))) &
               0x000000FF;
  return ((out_a << 0x18) | (out_r << 0x10) | (out_g << 0x08) |
          (out_b << 0x00));
}

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

env2d_conobject::env2d_conobject() : env2d_uiobject(), playback{}, dirty(true) {
  // TODO: dynamic fonts
  font = TTF_OpenFont("/usr/share/fonts/gnu-free/FreeMono.otf", 15);
}

void env2d_conobject::tick() {
  if (!dirty)
    return;
  dirty = false;
  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      framebuffer[i * w + j] = 0x10101010;
    }
  }

  // very sloppy math ahead
  if (playback.empty()) {
    return;
  }
  U8 sr, sg, sb, sa;
  auto off = 0;
  auto size = S32(playback.size());

  for (S32 i = size - 23; i < size; i++) {
    if (i < 0) {
      goto full;
    }
    if (!playback.at(i)->str.empty()) {
      switch (playback.at(i)->sev) {
      case severity::warn: {
        surf = TTF_RenderText_Blended(font, playback.at(i)->str.c_str(),
                                      SDL_Color{191, 191, 191, 0x10});
        break;
      }
      case severity::error: {
        surf = TTF_RenderText_Blended(font, playback.at(i)->str.c_str(),
                                      SDL_Color{255, 0, 0, 0x10});
        break;
      }
      default: {
        surf = TTF_RenderText_Blended(font, playback.at(i)->str.c_str(),
                                      SDL_Color{0, 127, 0, 0x10});
        break;
      }
      }
      if (surf == nullptr) {
        log(severity::error, {"TTF: ", TTF_GetError()});
      }
      assert(surf != nullptr);
      SDL_LockSurface(surf);
      auto pixels = reinterpret_cast<U8 *>(surf->pixels);
      for (auto i = 0; i < surf->h; i++) {
        for (auto j = 0; j < surf->w; j++) {
          auto index = i * surf->pitch + (j << 2);
          SDL_GetRGBA(*reinterpret_cast<U32 *>(pixels + index), surf->format,
                      &sr, &sg, &sb, &sa);
          auto dst = (U32(sa) << 0x18) | (U32(sr) << 0x10) | (U32(sg) << 0x08) |
                     (U32(sb) << 0x00);
          framebuffer[(i + off) * w + j] =
              calc_alpha2d(dst, framebuffer[(i + off) * w + j]);
        }
      }
      SDL_UnlockSurface(surf);
      SDL_FreeSurface(surf);
    }
  full:
    off += 15;
  }
  surf = TTF_RenderText_Blended(font, "lua>_", SDL_Color{0, 255, 0});
  if (surf == nullptr) {
    log(severity::error, {"TTF: ", TTF_GetError()});
  }
  assert(surf != nullptr);
  SDL_LockSurface(surf);
  auto pixels = reinterpret_cast<U8 *>(surf->pixels);
  for (auto i = 0; i < surf->h; i++) {
    for (auto j = 0; j < surf->w; j++) {
      auto index = i * surf->pitch + (j << 2);
      SDL_GetRGBA(*reinterpret_cast<U32 *>(pixels + index), surf->format, &sr,
                  &sg, &sb, &sa);
      auto dst = (U32(sa) << 0x18) | (U32(sr) << 0x10) | (U32(sg) << 0x08) |
                 (U32(sb) << 0x00);
      framebuffer[(i + off) * w + j] =
          calc_alpha2d(dst, framebuffer[(i + off) * w + j]);
    }
  }
  SDL_UnlockSurface(surf);
  SDL_FreeSurface(surf);
}

env2d_conobject::~env2d_conobject() {}