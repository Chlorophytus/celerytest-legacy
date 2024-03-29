#pragma once
// Lua-to-C++ API calls
#include "celerytest.hpp"
#include "celerytest_sim.hpp"

namespace celerytest {
namespace lua {
void declare_function(lua_State *, const char *, lua_CFunction);
sim::object *quick_getobj(lua_State *);
// Lua calls...
[[maybe_unused]] int log(lua_State *);
[[maybe_unused]] int create(lua_State *);
[[maybe_unused]] int create_glcontext(lua_State *);
[[maybe_unused]] int set_root_view(lua_State *);
[[maybe_unused]] int get_sim_time(lua_State *);
[[maybe_unused]] int poll(lua_State *);
[[maybe_unused]] int render(lua_State *);
[[maybe_unused]] int sleep(lua_State *);
[[maybe_unused]] int get(lua_State *);
[[maybe_unused]] int set(lua_State *);
[[maybe_unused]] int gui_insert(lua_State *);
[[maybe_unused]] int gui_objcnt(lua_State *);
[[maybe_unused]] int gui_remove(lua_State *);
[[maybe_unused]] int remove_glcontext(lua_State *);
[[maybe_unused]] int remove(lua_State *);
} // namespace lua
} // namespace celerytest
