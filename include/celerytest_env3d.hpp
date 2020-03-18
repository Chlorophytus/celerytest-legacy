#pragma once
#include "celerytest.hpp"
#include "celerytest_sim.hpp"

namespace celerytest {
enum class env3d_types : U8 {
  shaderprogram, // Done~
  displaylist,   // TODO
};
struct env3d_shaderprogram : sim_object {
  enum class types : U8 { compute, vertex, fragment, geometry };
  std::forward_list<GLuint> shaders;
  std::forward_list<std::string_view> sources;
  std::forward_list<char *> raws;
  virtual const sim_types get_type() const { return sim_types::env3duiobject; }
  virtual const env3d_types get_subtype() const {
    return env3d_types::shaderprogram;
  }
  void attach(std::string_view &&, types);
  void link();
  virtual void tick();
  bool has_linked, main_program;
  GLuint program, ticks;

  env3d_shaderprogram();
  ~env3d_shaderprogram();
};
} // namespace celerytest