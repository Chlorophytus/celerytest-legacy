#pragma once
// GUI objects in 2D space
#include "celerytest.hpp"
#include "celerytest_sim.hpp"
namespace celerytest {
namespace gui {
struct ctrl;
struct shader2d;
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
template <> struct is_object<gui::shader2d> {
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
template <> struct introspect_type<gui::shader2d> {
  constexpr const static char *value = "GuiShader2D";
};
} // namespace sim
namespace gui {
/* TODO: WIP Object, but I might overhaul the entire GUI system for a...
 * ...shader-based one*/
struct shader2d : sim::object {
  GLuint shader_program;
  GLuint shader_object;
  std::string source;

  sim::types get_type() const override { return sim::types::gui_shader2d; }
  const char *get_type_string() const override {
    return sim::introspect_type<shader2d>::value;
  }
};
struct ctrl : sim::object {
  bool dirty = false;
  SDL_Surface *surface;
  std::unique_ptr<SDL_Rect> rect{};

  std::optional<shader2d> shader{std::nullopt};

  sim::types get_type() const override { return sim::types::gui_ctrl; }
  const char *get_type_string() const override {
    return sim::introspect_type<ctrl>::value;
  }

  virtual void on_paint() {}
  void blit_here(SDL_Surface *) const;
};
struct text_ctrl : ctrl {
  std::string text;
  SDL_Color color{.r = 0xFF, .g = 0x00, .b = 0xFF, .a = 0xFF};

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