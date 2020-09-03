#pragma once
// GUI objects in 2D space
#include "celerytest.hpp"
#include "celerytest_sim.hpp"
namespace celerytest {
namespace gui {
struct ctrl;
struct text_ctrl;
}
namespace sim {
template <> struct is_object<gui::ctrl> {
  constexpr const static bool value = true;
};
template <> struct is_object<gui::text_ctrl> {
  constexpr const static bool value = true;
};
template <> struct introspect_type<gui::ctrl> {
  constexpr const static char *value = "GuiCtrl";
};
template <> struct introspect_type<gui::text_ctrl> {
  constexpr const static char *value = "GuiTextCtrl";
};
} // namespace sim
namespace gui {
struct ctrl : sim::object {
  bool dirty = false;
  SDL_Surface *surface;
  std::unique_ptr<SDL_Rect> rect{};
  sim::types get_type() const override { return sim::types::gui_ctrl; }
  const char *get_type_string() const override { return sim::introspect_type<ctrl>::value; }

  virtual void on_paint() {}
  void blit_here(SDL_Surface *) const;
};
struct text_ctrl : ctrl {
  std::string text = "Sample Text";
  SDL_Color color{
      .r = 0xFF,
      .g = 0x00,
      .b = 0xFF,
      .a = 0xFF
  };

  bool free_this = false;
  std::filesystem::path font_path;
  U16 font_size;
  TTF_Font *font;

  sim::types get_type() const override { return sim::types::gui_textctrl; }
  const char *get_type_string() const override { return sim::introspect_type<text_ctrl>::value; }

  void post_create() override;
  void on_paint() override;
  void pre_destroy() override;
};
} // namespace gui
} // namespace celerytest