#pragma once
// A "view" object represented w/FBOs, etc.
#include "celerytest.hpp"
#include "celerytest_gui.hpp"
#include "celerytest_sim.hpp"
namespace celerytest {
namespace glview {
struct view2d;
}
namespace sim {
template <> struct is_object<glview::view2d> {
  constexpr const static bool value = true;
};
template <> struct introspect_type<glview::view2d> {
  constexpr const static char *value = "GLView2D";
};
} // namespace sim
namespace glview {
struct view2d : sim::object {
  sim::session *parent = nullptr;
  std::vector<U64> ctrls_idx{};
  U16 w;
  U16 h;
  std::filesystem::path font_dir;

  sim::types get_type() const override { return sim::types::glview_view2d; }
  const char *get_type_string() const override {
    return sim::introspect_type<view2d>::value;
  }

  void post_create() override;
  virtual void paint();
  void render();
  void pre_destroy() override;

  GLuint framebuffer, ui_texture;
  SDL_Surface *surface;

  U32 t0, t1, last_ms;

  // Custom debugging stuff
  TTF_Font *font;
};
} // namespace glview
} // namespace celerytest