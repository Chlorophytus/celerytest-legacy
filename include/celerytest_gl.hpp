#pragma once
// OpenGL/SDL2 Session
#include "celerytest.hpp"
#include "celerytest_glview.hpp"

namespace celerytest {
namespace gl {
void create_context(std::string_view &, U16 &&, U16 &&, bool &&);
void set_root_view(glview::view2d *);
void tick();
void remove_context();
} // namespace gl
} // namespace celerytest