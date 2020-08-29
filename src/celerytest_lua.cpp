#include "../include/celerytest_con.hpp"
#include "../include/celerytest_gl.hpp"
#include "../include/celerytest_glview.hpp"

#include "../include/celerytest_lua.hpp"

using namespace celerytest;

// Sets a Lua function
void lua::set_one_function(lua_State *L, const char *key, lua_CFunction val) {
  lua_pushstring(L, key);    // push a key at -2
  lua_pushcfunction(L, val); // push a value at -3
  lua_settable(L, -3);
}

// === LUA FUNCTION CALLS =====================================================
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
  auto opt = luaL_checkoption(
      L, 1, nullptr,
      (const char *const[]){sim::introspect_type<sim::object>::value,
                            sim::introspect_type<glview::view2d>::value,
                            sim::introspect_type<glview::view3d>::value,
                            nullptr});
  switch (opt) {
  case 0: {
    auto idx = session.create_object<sim::object>();
    lua_pushinteger(L, idx);
    break;
  }
  case 1: {
    auto idx = session.create_object<glview::view2d>();
    auto o_ptr = session.query_object(idx);
    auto d_ptr = dynamic_cast<glview::view2d *>(o_ptr);
    d_ptr->w = luaL_checkinteger(L, 2);
    d_ptr->h = luaL_checkinteger(L, 3);
    d_ptr->post_create();

    lua_pushinteger(L, idx);
    break;
  }
    //  case 2: {
    //    auto idx = session.create_object<glview::view3d>();
    //    lua_pushinteger(L, idx);
    //    break;
    //  }
  default: {
    lua_pushnil(L);
    break;
  }
  }

  return 1;
}

[[maybe_unused]] int lua::create_glcontext(lua_State *L) {
  auto title = std::string_view{luaL_checkstring(L, 1)};
  auto w = luaL_checkinteger(L, 2);
  auto h = luaL_checkinteger(L, 3);
  auto f = lua_toboolean(L, 4);

  gl::create_context(title, w, h, f);

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
  auto o_ptr = session.query_object(idx);
  if (o_ptr != nullptr) {
    o_ptr->pre_destroy();
  }
  session.delete_object(idx);

  return 1;
}
[[maybe_unused]] int lua::get_sim_time(lua_State *L) {
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));

  lua_pushinteger(L, session.sim_time);
  return 1;
}

[[maybe_unused]] int lua::render(lua_State *L) {
  gl::tick();
  return 0;
}

[[maybe_unused]] int lua::poll(lua_State *L) {
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));
  session.tick();
  if (!session.pending.empty()) {
    auto &&event = session.pending.front();
    switch (event.data->type) {
    case SDL_KEYUP: {
      lua_newtable(L);

      lua_pushstring(L, SDL_GetKeyName(event.data->key.keysym.sym));
      lua_setfield(L, -2, "kname");

      lua_pushstring(L, "key_up");
      lua_setfield(L, -2, "type");

      lua_pushinteger(L, event.time);
      lua_setfield(L, -2, "time");

      session.pending.pop();
      return 1;
    }
    case SDL_QUIT: {
      lua_newtable(L);

      lua_pushstring(L, "quit");
      lua_setfield(L, -2, "type");

      lua_pushinteger(L, event.time);
      lua_setfield(L, -2, "time");

      session.pending.pop();
      return 1;
    }
    default: {
      lua_pushnil(L);
      session.pending.pop();
      return 1;
    }
    }
  }
  lua_pushnil(L);
  return 1;
}
int lua::sleep(lua_State *L) {
  auto ms = luaL_checkinteger(L, 1);
  SDL_Delay(ms);
  return 0;
}
int lua::set_root_view(lua_State *L) {
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));
  auto idx = luaL_checkinteger(L, 1);
  auto o_ptr = session.query_object(idx);

  if (o_ptr != nullptr) {
    if (o_ptr->get_type() == sim::types::glview_view2d ||
        o_ptr->get_type() == sim::types::glview_view3d) {
      auto r_ptr = dynamic_cast<glview::view2d *>(o_ptr);
      gl::set_root_view(r_ptr);
    }
  }
  return 0;
}
