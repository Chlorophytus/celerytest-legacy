#pragma once
// GUI objects in 2D space
#include "celerytest.hpp"
#include "celerytest_sim.hpp"
namespace celerytest {
namespace gui {
struct ctrl;
struct text_ctrl;
struct image_ctrl;
} // namespace gui
namespace sim {
template <> struct is_object<gui::ctrl> {
  constexpr const static bool value = true;
};
template <> struct is_object<gui::text_ctrl> {
  constexpr const static bool value = true;
};
template <> struct is_object<gui::image_ctrl> {
  constexpr const static bool value = true;
};
template <> struct introspect_type<gui::ctrl> {
  constexpr const static char *value = "GuiCtrl";
};
template <> struct introspect_type<gui::text_ctrl> {
  constexpr const static char *value = "GuiTextCtrl";
};
template <> struct introspect_type<gui::image_ctrl> {
  constexpr const static char *value = "GuiImageCtrl";
};
} // namespace sim
namespace gui {
enum class uniform_types : U8 {
  float4,
  int4,
  uint4,
};
struct uniform {
  uniform_types type;
  union {
    GLfloat f[4];
    GLint i[4];
    GLuint u[4];
  } data;
};
struct filter2d {
  GLuint shader_program;
  GLuint shader_object;
  GLuint textures[2]; // 0 = src, 1 = dst
  std::map<U16, uniform> uniforms{}; // Uniforms start off at location 2

  std::string source;
};
struct ctrl : sim::object {
  bool dirty = false;
  SDL_Surface *surface;
  SDL_Surface *filtered;
  std::unique_ptr<SDL_Rect> rect{};

  std::optional<filter2d> filter{std::nullopt};

  sim::types get_type() const override { return sim::types::gui_ctrl; }
  const char *get_type_string() const override {
    return sim::introspect_type<ctrl>::value;
  }

  void post_create() override;
  virtual void on_paint(){};
  void pre_destroy() override;

  void do_filter();
  void blit_here(SDL_Surface *) const;
};
struct text_ctrl : ctrl {
  std::string text;
  SDL_Color color{};

  bool free_this = false;
  std::filesystem::path font_path;
  U16 font_size;
  TTF_Font *font;

  sim::types get_type() const override { return sim::types::gui_textctrl; }
  const char *get_type_string() const override {
    return sim::introspect_type<text_ctrl>::value;
  }

  void post_create() override;
  void on_paint() override;
  void pre_destroy() override;
};
struct image_ctrl : ctrl {
  bool free_this = false;
  std::filesystem::path image_path;

  void post_create() override;
  void on_paint() override;
  void pre_destroy() override;
};
} // namespace gui
} // namespace celerytest