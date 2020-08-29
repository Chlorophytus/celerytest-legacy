#pragma once
// A "view" object represented w/FBOs, etc.
#include "celerytest.hpp"
#include "celerytest_sim.hpp"
namespace celerytest {
namespace glview {
struct view2d : sim::object {
  U16 w;
  U16 h;

  sim::types get_type() const override { return sim::types::glview_view2d; }
  const char *get_type_string() const override { return "GLView2D"; }

  void post_create() override;
  virtual void onrender();
  void render();
  void pre_destroy() override;

  GLuint framebuffer, ui_texture;
  SDL_Surface *surface;

  U32 t0, t1, last_ms;

  // Custom debugging stuff
  TTF_Font *font;
};
struct view3d : view2d {
  sim::types get_type() const override { return sim::types::glview_view3d; }
  const char *get_type_string() const override { return "GLView3D"; }

  void post_create() override;
  void pre_destroy() override;
};
} // namespace glview
namespace sim {
template <> struct is_object<glview::view2d> {
  constexpr const static bool value = true;
};
template <> struct introspect_type<glview::view2d> {
  constexpr const static char *value = "GLView2D";
};
template <> struct is_object<glview::view3d> {
  constexpr const static bool value = true;
};
template <> struct introspect_type<glview::view3d> {
  constexpr const static char *value = "GLView3D";
};
} // namespace sim
} // namespace celerytest