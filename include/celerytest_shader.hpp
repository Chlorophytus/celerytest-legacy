#pragma once
#include "celerytest.hpp"

namespace celerytest {
struct shader {
  enum class type : U8 { compute, vertex, fragment, geometry };
  type shader_type;
  std::string_view shader_source;
  char *raw;
  GLuint idx;
  shader(type, std::string_view &&);
  ~shader();
};
struct shader_chain {
  std::forward_list<shader> chain;
  GLuint idx;
  shader_chain(std::forward_list<shader> &&);
  ~shader_chain();
};
} // namespace celerytest
