#include "../include/celerytest_lua.hpp"
using namespace celerytest;

lua::lua() {
  L = luaL_newstate();
  assert(L);
}

lua::~lua() { lua_close(L); }