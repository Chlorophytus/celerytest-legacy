#pragma once
#include "celerytest.hpp"
#include "celerytest_sim.hpp"

namespace celerytest {
enum class env3d_types : U8 {
  object,
  shaderprogram, // TODO
  shaderopaque,  // Done?
  displaylist,   // TODO
};
struct env3d_uiobject : sim_object {
  virtual const sim_types get_type() const { return sim_types::env3duiobject; }
  virtual const env3d_types get_subtype() const {
    return env3d_types::object;
  }
};
struct env3d_shaderopaque : env3d_uiobject {
  enum class types : U8 { compute, vertex, fragment, geometry };
  GLuint shader;
  std::string_view source;
  char *raw_source;
  types type;
  virtual const env3d_types get_subtype() const {
    return env3d_types::shaderopaque;
  }
  bool has_compiled;
  void compile(std::string_view &&, types);
  env3d_shaderopaque();
  ~env3d_shaderopaque();
};
struct env3d_shaderprogram : env3d_uiobject {
  std::forward_list<env3d_shaderopaque> opaques;
  virtual const env3d_types get_subtype() const {
    return env3d_types::shaderprogram;
  }
  void link();
  virtual void tick();
  bool has_linked, main_program;
  GLuint program, ticks;

  env3d_shaderprogram();
  ~env3d_shaderprogram();
};
} // namespace celerytest