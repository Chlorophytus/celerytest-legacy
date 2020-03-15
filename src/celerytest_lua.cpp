#include "../include/celerytest_lua.hpp"
#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_shader.hpp"
#include "../include/celerytest_sim.hpp"
using namespace celerytest;

const auto root = std::filesystem::path(".");
auto env2d = std::unique_ptr<std::vector<U32>>{nullptr};
auto con2d = std::unique_ptr<U32>{nullptr};

U32 celerytest::get_con2d() {
  if (!con2d) {
    log(severity::info, {"Creating 2D console..."});
    auto what = celerytest::sim_create(5);
    con2d = std::make_unique<U32>(what);
    auto rconsole = celerytest::sim_reference(*con2d);
    assert(rconsole->get_type() == sim_types::env2duiobject);
    assert(
        dynamic_cast<celerytest::env2d_uiobject *>(rconsole)->get_subtype() ==
        env2d_types::console);

    auto console = dynamic_cast<celerytest::env2d_conobject *>(rconsole);
    console->fill(480, 360, 0, 0);
    // OHNO: could be dangerous
    get_env2d()->emplace_back(*con2d);
    log(severity::info, {"Created 2D console..."});
  }
  return *con2d;
}

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

      // Create a 2d console.

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
      log(severity::info, {"creating shaderlist..."});
      auto ref = celerytest::sim_reference(what);
      assert(ref->get_type() == celerytest::sim_types::shaderlist);
      auto &&casted = dynamic_cast<celerytest::sim_shaderlist *>(ref);
      casted->primary = std::make_unique<celerytest::shader_chain>();
      for (auto i = 1;; i++) {
        auto type = lua_geti(L, 3, i);
        if (type == LUA_TNIL) {
          lua_pop(L, 1);
          break;
        }
        if (type == LUA_TNUMBER) {
          auto idx = lua_tointeger(L, -1);

          lua_pop(L, 1);
          auto sh = celerytest::sim_reference(idx);
          assert(sh->get_type() == celerytest::sim_types::shaderobject);
          casted->primary->shader_idxs.emplace_front(
              dynamic_cast<celerytest::sim_shaderobject *>(sh)->primary->idx);
        }
      }
      casted->primary->shader_idxs.reverse();
      casted->primary->link();
      break;
    }
    case 4: {
      log(severity::info, {"creating 2d ui element..."});

      lua_getfield(L, 3, "x");
      auto x = lua_tointeger(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 3, "y");
      auto y = lua_tointeger(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 3, "w");
      auto w = lua_tointeger(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 3, "h");
      auto h = lua_tointeger(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 3, "zlevel");
      auto z = lua_tointeger(L, -1);
      lua_pop(L, 1);

      auto ref = celerytest::sim_reference(what);
      assert(ref->get_type() == celerytest::sim_types::env2duiobject);
      auto &&casted = dynamic_cast<celerytest::env2d_uiobject *>(ref);
      casted->fill(w, h, x, y);
      get_env2d()->emplace(get_env2d()->begin() + z, what);
      log(severity::info,
          {"ui2d w=", std::to_string(w), ", w=", std::to_string(h),
           ", x=", std::to_string(x), ", y=", std::to_string(y)});
      break;
    }
    }
  }
  lua_pushinteger(L, what);
  return 1;
}
int lua::sim_delete(lua_State *L) {
  auto type = luaL_checkinteger(L, 1);
  celerytest::sim_delete(type);
  return 0;
}

lua::~lua() { lua_close(L); }

std::vector<U32> *celerytest::get_env2d() {
  if (!env2d) {
    celerytest::log(celerytest::severity::info,
                    {"Initializing 2D environment..."});
    env2d = std::make_unique<std::vector<U32>>();
    celerytest::log(celerytest::severity::info,
                    {"Initialized 2D environment..."});
  }
  return env2d.get();
}