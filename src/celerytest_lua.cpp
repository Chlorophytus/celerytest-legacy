#include "../include/celerytest_con.hpp"

#include "../include/celerytest_lua.hpp"

using namespace celerytest;

// Sets a Lua function
void lua::set_one_function(lua_State *L, const char *key, lua_CFunction val) {
  lua_pushstring(L, key);    // create table at -1
  lua_pushcfunction(L, val); // push a key at -2
  lua_settable(L, -3);       // push a value at -3
}

// Sets all Lua functions, initializes the Celerytest table
void lua::set_all_functions(lua_State *L) {
  con::log_all(con::severity::debug, {"setting all lua functions"});
  lua_newtable(L);

  // SET FUNCTIONS BELOW

  lua::set_one_function(L, "log", lua::log);

  // SET FUNCTIONS ABOVE

  lua_setglobal(L, "celerytest");
  con::log_all(con::severity::debug, {"setted all lua functions"});
}

// === LUA FUNCTION CALLS =====================================================
int lua::start_gui(lua_State *L) { return 0; }

int lua::log(lua_State *L) {
  auto severity =
      luaL_checkoption(L, 1, "inf",
                       (const char *const[]){"emg", "alt", "crt", "err", "wrn",
                                             "not", "inf", "deb", nullptr});
  auto message = luaL_checkstring(L, 2);
  switch (severity) {
  case 0: {
    con::log_all(con::severity::emergency, {message});
    break;
  }
  case 1: {
    con::log_all(con::severity::alert, {message});
    break;
  }
  case 2: {
    con::log_all(con::severity::critical, {message});
    break;
  }
  case 3: {
    con::log_all(con::severity::error, {message});
    break;
  }
  case 4: {
    con::log_all(con::severity::warning, {message});
    break;
  }
  case 5: {
    con::log_all(con::severity::notice, {message});
    break;
  }
  case 6: {
    con::log_all(con::severity::informational, {message});
    break;
  }
  case 7: {
    con::log_all(con::severity::debug, {message});
    break;
  }
  default: {
    break;
  }
  }
  return 0;
}