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
  std::forward_list<GLuint> shader_idxs;
  GLuint idx;
  void link();
  shader_chain();
  ~shader_chain();
};
} // namespace celerytest
