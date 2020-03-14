#include "../include/celerytest_lua.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_sim.hpp"
using namespace celerytest;

const auto root = std::filesystem::path(".");

lua::lua() {
  L = luaL_newstate();
  assert(L);
  luaL_openlibs(L);

  lua_pushcfunction(L, con_info);
  lua_setglobal(L, "__con_info");
  lua_pushcfunction(L, con_warn);
  lua_setglobal(L, "__con_warn");
  lua_pushcfunction(L, con_error);
  lua_setglobal(L, "__con_error");
  lua_pushcfunction(L, sim_create);
  lua_setglobal(L, "__sim_create");
  lua_pushcfunction(L, sim_create_hint);
  lua_setglobal(L, "__sim_create_hint");
  lua_pushcfunction(L, sim_delete);
  lua_setglobal(L, "__sim_delete");
}

void lua::load(const char *path) {
  auto luapath = root / "lua";
  luapath /= path;
  log(severity::warn, {"Loading Lua file '", luapath.c_str(), "'"});
  int err = luaL_dofile(L, luapath.c_str());
  switch (err) {
  case LUA_OK:
    log(severity::info, {"Load '", luapath.c_str(), "' successful."});
    break;
  case LUA_ERRSYNTAX:
    log(severity::error,
        {"Load '", luapath.c_str(), "' failed, syntax error."});
    break;
  case LUA_ERRFILE:
    log(severity::error, {"Load '", luapath.c_str(), "' failed, file error."});
    break;
  case LUA_ERRMEM:
    log(severity::error,
        {"Load '", luapath.c_str(), "' failed, memory error."});
    break;
  case LUA_ERRGCMM:
    log(severity::error,
        {"Load '", luapath.c_str(), "' failed, garbage collector error."});
    break;
  default:
    log(severity::error,
        {"Load '", luapath.c_str(), "' failed, unknown error."});
    break;
  }
  assert(err == LUA_OK);
}

int lua::con_info(lua_State *L) {
  auto string = luaL_checkstring(L, 1);
  log(severity::info, {"script: ", string});
  return 0;
}

int lua::con_warn(lua_State *L) {
  auto string = luaL_checkstring(L, 1);
  log(severity::warn, {"script: ", string});
  return 0;
}

int lua::con_error(lua_State *L) {
  auto string = luaL_checkstring(L, 1);
  log(severity::error, {"script: ", string});
  return 0;
}

int lua::sim_create(lua_State *L) {
  auto type = luaL_checkinteger(L, 1);
  auto what = celerytest::sim_create(type);
  lua_pushnumber(L, what);
  return 1;
}
int lua::sim_create_hint(lua_State *L) {
  auto type = luaL_checkinteger(L, 1);
  auto hint = luaL_checkinteger(L, 2);
  auto what = celerytest::sim_create_hint(type, hint);
  lua_pushnumber(L, what);
  return 1;
}
int lua::sim_delete(lua_State *L) {
  auto type = luaL_checkinteger(L, 1);
  celerytest::sim_delete(type);
  return 0;
}

lua::~lua() { lua_close(L); }