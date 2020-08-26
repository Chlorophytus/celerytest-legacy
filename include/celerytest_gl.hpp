#pragma once
// OpenGL/SDL2 Session
#include "celerytest.hpp"

namespace celerytest {
namespace gl {
void create_context(std::string_view &, U16 &&, U16 &&, bool &&);
[[maybe_unused]] bool tick();
void remove_context();
} // namespace gl
} // namespace celerytest