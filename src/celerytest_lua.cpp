#include "../include/celerytest_lua.hpp"
#include "../include/celerytest_env2d.hpp"
#include "../include/celerytest_env3d.hpp"
#include "../include/celerytest_sim.hpp"
using namespace celerytest;

const auto root = std::filesystem::path(".");
auto env2d = std::unique_ptr<std::vector<U32>>{nullptr};
auto con2d = std::unique_ptr<U32>{nullptr};
auto kmaps = std::unique_ptr<kmaps_t>{nullptr};

void celerytest::con2d_log(celerytest::severity s,
                           std::forward_list<std::string_view> &&msg) {
  auto rconsole = celerytest::sim_reference(*con2d);
  assert(rconsole->get_type() == sim_types::env2duiobject);
  assert(dynamic_cast<celerytest::env2d_uiobject *>(rconsole)->get_subtype() ==
         env2d_types::console);
  auto console = dynamic_cast<celerytest::env2d_conobject *>(rconsole);
  auto cnt = 0;
  auto f = std::string("");
  for (auto m : msg) {
    cnt = 0;
    for (auto c : m) {
      if (cnt > 50) {
        break;
      }
      f.push_back(c);
      cnt++;
    }
  }
  console->playback.emplace_back(new celerytest::env2d_conentry{s, f});
  console->dirty = true;
}

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
    console->fill(480, 360, 0, 0, nullptr);
    // OHNO: could be dangerous
    console->dirty = true;
    console->show = false;
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
  lua_pushcfunction(L, kmaps_declare);
  lua_setglobal(L, "__kmaps_declare");
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

kmaps_t *get_kmap() {
  if (!kmaps) {
    log(severity::warn, {"Making kmap..."});
    kmaps = std::make_unique<kmaps_t>();
  }
  return kmaps.get();
}

int lua::kmaps_declare(lua_State *L) {
  auto key = luaL_checkinteger(L, 1);
  auto event = luaL_checkstring(L, 2);
  log(severity::warn, {"Binding keycode: ", std::to_string(key)});
  get_kmap()->insert_or_assign(key, std::string_view{event});
  return 0;
}

void lua::kmaps_call(lua_State *L, SDL_Keycode keycode) {
  switch (keycode) {
  case '`': {
    auto &&con = dynamic_cast<env2d_conobject *>(sim_reference(get_con2d()));
    if (con->show) {
      con->show = false;
    } else {
      con->show = true;
    }
    break;
  }
  case SDLK_BACKSPACE: {
    auto &&con = dynamic_cast<env2d_conobject *>(sim_reference(get_con2d()));
    if (con->show && !con->curr_prompt.empty()) {
      con->curr_prompt.pop_back();
      con->dirty = true;
    }
    break;
  }
  case SDLK_RETURN: {
    auto &&con = dynamic_cast<env2d_conobject *>(sim_reference(get_con2d()));
    if (con->show && !con->curr_prompt.empty()) {
      lua_pushstring(L, con->curr_prompt.c_str());
      con->curr_prompt = "";
      if (lua_pcall(L, 0, 0, 0) != 0) {
        auto err = std::string{lua_tostring(L, -1)};
        lua_pop(L, 1);
        log(severity::error, {"lua: ", err});
      }
      con->dirty = true;
    }
    break;
  }
  default: {
    if ((keycode & (1 << 30)) != 0) {
      if (get_kmap()->count(keycode) > 0) {
        auto &&key = get_kmap()->find(keycode);
        lua_getglobal(L, key->second.c_str());
        lua_call(L, 0, 0);
      }
    } else {
      auto &&con = dynamic_cast<env2d_conobject *>(sim_reference(get_con2d()));
      if (con->show) {
        con->curr_prompt.push_back(keycode);
        con->dirty = true;
      }
    }
    break;
  }
  }
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
    case 2: {
      log(severity::error, {"You are creating a deprecated object. Stop."});
      assert(false);
      break;
    }
    case 3: {
      log(severity::error, {"You are creating a deprecated object. Stop."});
      assert(false);
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

      lua_getfield(L, 3, "zlevel");
      auto z = lua_tointeger(L, -1);
      lua_pop(L, 1);

      auto ref = celerytest::sim_reference(what);
      assert(ref->get_type() == celerytest::sim_types::env2duiobject);
      auto &&casted = dynamic_cast<celerytest::env2d_uiobject *>(ref);

      lua_getfield(L, 3, "image");
      if (!lua_isnoneornil(L, -1)) {
        // since our image already has width and height vals just use dummy
        // vals.
        auto pstr = lua_tostring(L, -1);
        lua_pop(L, 1);
        casted->fill(-1, -1, x, y, (root / pstr).c_str());
      } else {
        // of course a procedurally generated ui needs W and H vals
        lua_getfield(L, 3, "w");
        auto w = lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 3, "h");
        auto h = lua_tointeger(L, -1);
        lua_pop(L, 1);

        casted->fill(w, h, x, y, nullptr);
      }
      get_env2d()->emplace(get_env2d()->begin() + z, what);
      break;
    }
    case 6: {
      lua_getfield(L, 3, "is_main");
      auto is_main = lua_toboolean(L, -1);
      lua_pop(L, 1);
      auto ref = celerytest::sim_reference(what);
      assert(ref->get_type() == celerytest::sim_types::env3duiobject);
      auto casted = dynamic_cast<celerytest::env3d_uiobject *>(ref);
      assert(casted->get_subtype() == celerytest::env3d_types::shaderprogram);
      auto shader_program =
          dynamic_cast<celerytest::env3d_shaderprogram *>(casted);
      shader_program->main_program = is_main;

      lua_getfield(L, 3, "shaders");
      for (auto i = 1;; i++) {
        auto type = lua_geti(L, -1, i);
        if (type == LUA_TNIL) {
          lua_pop(L, 1);
          break;
        }
        if (type == LUA_TNUMBER) {
          auto idx = lua_tointeger(L, -1);
          lua_pop(L, 1);

          auto sh = celerytest::sim_reference(idx);
          assert(sh->get_type() == celerytest::sim_types::env3duiobject);

          auto casted = dynamic_cast<celerytest::env3d_uiobject *>(sh);
          assert(casted->get_subtype() ==
                 celerytest::env3d_types::shaderopaque);

          auto opaque = dynamic_cast<celerytest::env3d_shaderopaque *>(casted);
          shader_program->opaques.emplace_front(*opaque);
        }
      }
      lua_pop(L, 1);
      shader_program->link();
      break;
    }
    case 7: {
      lua_getfield(L, 3, "source");
      auto source = lua_tostring(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, 3, "type");
      auto shader_type = lua_tointeger(L, -1);
      lua_pop(L, 1);
      auto ref = celerytest::sim_reference(what);
      assert(ref->get_type() == celerytest::sim_types::env3duiobject);
      auto casted = dynamic_cast<celerytest::env3d_uiobject *>(ref);
      assert(casted->get_subtype() == celerytest::env3d_types::shaderopaque);
      auto opaque = dynamic_cast<celerytest::env3d_shaderopaque *>(casted);
      switch (shader_type) {
      case 1: {
        opaque->compile(source, celerytest::env3d_shaderopaque::types::compute);
        break;
      }
      case 2: {
        opaque->compile(source,
                        celerytest::env3d_shaderopaque::types::fragment);
        break;
      }
      case 3: {
        opaque->compile(source,
                        celerytest::env3d_shaderopaque::types::geometry);
        break;
      }
      case 4: {
        opaque->compile(source, celerytest::env3d_shaderopaque::types::vertex);
        break;
      }
      }
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