#pragma once
#include "celerytest.hpp"
#include "celerytest_log.hpp"
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
  static int con_toggle(lua_State *);

  // KMaps
  static int kmaps_declare(lua_State *);
  void kmaps_call(SDL_Keycode);

  // Sim
  static int sim_create(lua_State *);
  static int sim_delete(lua_State *);
};
std::vector<U32> *get_env2d();
U32 get_con2d();
kmaps_t *get_kmaps();
void con2d_log(celerytest::severity, std::forward_list<std::string_view> &&);
} // namespace celerytest
