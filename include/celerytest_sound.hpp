#pragma once
#include "celerytest.hpp"

namespace celerytest {
struct sound_engine {
  U8 echodecay_l = 255;
  U8 echodecay_r = 255;
  U8 echobuffer_l[65536];                  // echobuffer L
  U8 echobuffer_r[65536];                  // echobuffer R
  void mix(U8 &, U8 &);
};
void sound_init();
void sound_callback(void *, Uint8 *, int);
void sound_deinit();
} // namespace celerytest