#pragma once
// Lua-to-C++ API calls
#include "celerytest.hpp"
#include "celerytest_sim.hpp"

namespace celerytest {
namespace lua {
void set_one_function(lua_State *, const char *, lua_CFunction);

// Lua calls...
[[maybe_unused]] int start_gui([[maybe_unused]] lua_State *);
int log(lua_State *);
int create(lua_State *);
[[maybe_unused]] int create_glcontext(lua_State *);
[[maybe_unused]] int sleep(lua_State *);
[[maybe_unused]] int remove_glcontext(lua_State *);
int remove(lua_State *);
} // namespace lua
} // namespace celerytest
