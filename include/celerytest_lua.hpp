#pragma once
#include "celerytest.hpp"
#include "celerytest_log.hpp"
#include "celerytest_mission.hpp"
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
  static int con_evaluate(lua_State *);

  // KMaps
  static int kmaps_declare(lua_State *);
  void kmaps_call(lua_State *, SDL_Keycode, bool);

  // Sim
  static int sim_create(lua_State *);
  static int sim_delete(lua_State *);

  // Mission
  static int mission_change(lua_State *);
};
U32 get_mission(mission_types);
mission_types *get_missiontype();
std::vector<U32> *get_env2d();
U32 get_con2d();
kmaps_t *get_kmaps();
void con2d_log(celerytest::severity, std::forward_list<std::string_view> &&);
} // namespace celerytest
