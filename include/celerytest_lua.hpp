#pragma once
// Lua-to-C++ API calls
#include "celerytest.hpp"

namespace celerytest {
namespace lua {
void set_one_function(lua_State *, const char *, lua_CFunction);
void set_all_functions(lua_State *);

// Lua calls...
int start_gui(lua_State *);
int log(lua_State *);
} // namespace lua
} // namespace celerytest
