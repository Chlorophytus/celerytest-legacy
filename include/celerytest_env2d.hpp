#pragma once
#include "celerytest.hpp"
#include "celerytest_sim.hpp"

namespace celerytest {
U32 calc_alpha2d(U32, U32);
enum class env2d_types : U8 {
  object, // DONE
  console // DONE
};
struct env2d_uiobject : sim_object {
  U16 w, h, x, y;
  bool resize, dirty_before, empty_before;
  GLuint fbo, tex;
  U32 *framebuffer;
  virtual const sim_types get_type() const { return sim_types::env2duiobject; }
  virtual const env2d_types get_subtype() const { return env2d_types::object; }
  void fill(U16, U16, U16, U16);
  virtual void tick();

  env2d_uiobject();
  ~env2d_uiobject();
};
struct env2d_conobject : env2d_uiobject {
  virtual const env2d_types get_subtype() const { return env2d_types::console; }
  bool dirty;
  TTF_Font *font;
  SDL_Surface *surf;
  env2d_conobject();
  ~env2d_conobject();
  virtual void tick();
};
// TODO: resizeable Env2DUiObjects :)
} // namespace celerytest

// Placeholder