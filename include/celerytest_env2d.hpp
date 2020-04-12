#pragma once
#include "celerytest.hpp"
#include "celerytest_sim.hpp"
#include "celerytest_log.hpp"

namespace celerytest {
enum class env2d_types : U8 {
  object, // DONE
  console, // DONE
};
struct env2d_uiobject : sim_object {
  U16 w, h, x, y;
  bool empty_before, show, dirty;
  virtual const sim_types get_type() const { return sim_types::env2duiobject; }
  virtual const env2d_types get_subtype() const { return env2d_types::object; }
  void fill(U16, U16, U16, U16, const char *);
  virtual void tick();

  SDL_Surface *surf;

  const char *on_mouse;

  env2d_uiobject();
  ~env2d_uiobject();
};
struct env2d_conentry {
  celerytest::severity sev;
  std::string str;
};
struct env2d_conobject : env2d_uiobject {
  virtual const env2d_types get_subtype() const { return env2d_types::console; }
  TTF_Font *font;
  SDL_Surface *fontsurf;
  virtual void tick();
  std::vector<std::unique_ptr<env2d_conentry>> playback;
  std::string curr_prompt;
  env2d_conobject();
};
// TODO: resizeable Env2DUiObjects :)
} // namespace celerytest

// Placeholder