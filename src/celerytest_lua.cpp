#include "../include/celerytest_con.hpp"
#include "../include/celerytest_gl.hpp"

#include "../include/celerytest_lua.hpp"

using namespace celerytest;

// Sets a Lua function
void lua::set_one_function(lua_State *L, const char *key, lua_CFunction val) {
  lua_pushstring(L, key);    // push a key at -2
  lua_pushcfunction(L, val); // push a value at -3
  lua_settable(L, -3);
}

// === LUA FUNCTION CALLS =====================================================
[[maybe_unused]] int lua::start_gui([[maybe_unused]] lua_State *L) { return 0; }

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

int lua::create(lua_State *L) {
  // Get our table and its session back
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));

  // Now create the object
  auto idx = session.create_object<sim::object>();
  lua_pushinteger(L, idx);

  return 1;
}

[[maybe_unused]] int lua::create_glcontext(lua_State *L) {
  auto title = std::string_view{luaL_checkstring(L, 1)};
  auto w = luaL_checkinteger(L, 2);
  auto h = luaL_checkinteger(L, 3);

  gl::create_context(title, w, h);

  return 0;
}

[[maybe_unused]] int lua::remove_glcontext(lua_State *) {
  gl::remove_context();

  return 0;
}

int lua::remove(lua_State *L) {
  auto idx = luaL_checkinteger(L, 1);

  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));

  session.delete_object(idx);

  return 0;
}
int lua::sleep(lua_State *L) {
  auto ms = luaL_checkinteger(L, 1);
  SDL_Delay(ms);
  return 0;
}
