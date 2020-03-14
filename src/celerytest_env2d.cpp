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