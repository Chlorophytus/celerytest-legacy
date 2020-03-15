#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_log.hpp"
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
                   (src_a + (dst_a * (src_a)))) & 0x000000FF;
  auto out_g = U32(((src_g * src_a) + ((dst_g * dst_a) * (src_a))) /
                   (src_a + (dst_a * (src_a)))) & 0x000000FF;
  auto out_b = U32(((src_b * src_a) + ((dst_b * dst_a) * (src_a))) /
                   (src_a + (dst_a * (src_a)))) & 0x000000FF;
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

env2d_conobject::env2d_conobject() : env2d_uiobject(), dirty(true) {
  // TODO: dynamic fonts
  font = TTF_OpenFont("/usr/share/fonts/droid/DroidSansMono.ttf", 14);
  surf = TTF_RenderText_Blended(font, "There's a new kid in town...test",
                                SDL_Color{0x00, 0xC0, 0x00, 0xC0});
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
      framebuffer[i * w + j] = 0x10101010;
    }
  }

  // very sloppy math ahead
  SDL_LockSurface(surf);
  auto pixels = reinterpret_cast<U8 *>(surf->pixels);
  U8 sr, sg, sb, sa;
  for (auto i = 0; i < surf->h; i++) {
    for (auto j = 4; j < surf->w; j++) {
      auto index = i * surf->pitch + (j << 2);
      SDL_GetRGBA(*reinterpret_cast<U32 *>(pixels + index), surf->format, &sr,
                  &sg, &sb, &sa);
      auto dst = (U32(sa) << 0x18) | (U32(sr) << 0x10) | (U32(sg) << 0x08) |
                 (U32(sb) << 0x00);
      framebuffer[i * w + j] = calc_alpha2d(dst, framebuffer[i * w + j]);
    }
  }
  SDL_UnlockSurface(surf);
}

env2d_conobject::~env2d_conobject() { SDL_FreeSurface(surf); }