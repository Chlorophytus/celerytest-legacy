#pragma once
#include "celerytest.hpp"
#include "celerytest_sim.hpp"

namespace celerytest {
enum class env2d_types : U8 {
  object,    // DONE
  console    // TODO
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
// TODO: resizeable Env2DUiObjects :)
} // namespace celerytest

// Placeholder