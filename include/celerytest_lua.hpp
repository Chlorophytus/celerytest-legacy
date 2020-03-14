#pragma once
#include "celerytest.hpp"

namespace celerytest {
struct lua {
  lua_State *L;
  lua();
  void load(const char *);
  ~lua();

  // Con
  static int con_info(lua_State *);
  static int con_warn(lua_State *);
  static int con_error(lua_State *);

  // Sim
  static int sim_create(lua_State *);
  static int sim_create_hint(lua_State *);
  static int sim_delete(lua_State *);
};
} // namespace celerytest
