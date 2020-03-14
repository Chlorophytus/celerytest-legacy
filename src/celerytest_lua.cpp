#include "../include/celerytest_lua.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_shader.hpp"
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
  auto what = U32(0);
  if (lua_isnil(L, 2)) {
    what = celerytest::sim_create(type);
  } else {
    auto hint = luaL_checkinteger(L, 2);
    what = celerytest::sim_create_hint(type, hint);
  }
  if (lua_istable(L, 3)) {
    switch (type) {
    case 1: {
      break;
    }
    case 2: {
      lua_getfield(L, 3, "type");
      auto shtype = luaL_checkinteger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, 3, "source");
      auto source = luaL_checkstring(L, -1);
      lua_pop(L, 1);
      auto ref = celerytest::sim_reference(what);
      assert(ref->get_type() == celerytest::sim_types::shaderobject);
      switch (shtype) {
      case 1: {
        log(severity::info, {"creating compute shader..."});
        dynamic_cast<celerytest::sim_shaderobject *>(ref)->primary =
            std::make_unique<celerytest::shader>(
                celerytest::shader::type::compute, source);
        break;
      }
      case 2: {
        log(severity::info, {"creating vertex shader..."});
        dynamic_cast<celerytest::sim_shaderobject *>(ref)->primary =
            std::make_unique<celerytest::shader>(
                celerytest::shader::type::vertex, source);
        break;
      }
      case 3: {
        log(severity::info, {"creating fragment shader..."});
        dynamic_cast<celerytest::sim_shaderobject *>(ref)->primary =
            std::make_unique<celerytest::shader>(
                celerytest::shader::type::fragment, source);
        break;
      }
      case 4: {
        log(severity::info, {"creating geometry shader..."});
        dynamic_cast<celerytest::sim_shaderobject *>(ref)->primary =
            std::make_unique<celerytest::shader>(
                celerytest::shader::type::geometry, source);
        break;
      }
      }
      break;
    }
    case 3: {
      // WIP
      lua_getfield(L, 3, "type");
      auto shtype = luaL_checkinteger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, 3, "source");
      auto source = luaL_checkstring(L, -1);
      lua_pop(L, 1);
      auto ref = celerytest::sim_reference(what);
      assert(ref->get_type() == celerytest::sim_types::shaderobject);
      dynamic_cast<celerytest::sim_shaderlist *>
      break;
    }
    }
  }
  lua_pushnumber(L, what);
  return 1;
}
int lua::sim_delete(lua_State *L) {
  auto type = luaL_checkinteger(L, 1);
  celerytest::sim_delete(type);
  return 0;
}

lua::~lua() { lua_close(L); }