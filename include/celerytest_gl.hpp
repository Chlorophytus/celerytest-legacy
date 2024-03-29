#pragma once
// OpenGL/SDL2 Session
#include "celerytest.hpp"
#include "celerytest_glview.hpp"

namespace celerytest {
namespace gl {
void create_context(std::string &&, U16 &&, U16 &&, bool &&);
void set_root_view(sim::session *, glview::view2d *);
void tick();
void remove_context();
glview::view2d *get_root_view();
U16 w();
U16 h();
} // namespace gl
} // namespace celerytest