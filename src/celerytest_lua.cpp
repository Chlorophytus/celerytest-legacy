#include "../include/celerytest_con.hpp"
#include "../include/celerytest_gl.hpp"
#include "../include/celerytest_glview.hpp"

#include "../include/celerytest_lua.hpp"

using namespace celerytest;

// Sets a Lua function
void lua::declare_function(lua_State *L, const char *key, lua_CFunction val) {
  lua_pushstring(L, key);    // push a key at -2
  lua_pushcfunction(L, val); // push a value at -3
  lua_settable(L, -3);
}

// === LUA FUNCTION CALLS =====================================================
[[maybe_unused]] int lua::log(lua_State *L) {
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

[[maybe_unused]] int lua::create(lua_State *L) {
  // Get our table and its session back
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));
  lua_pop(L, 2);

  // Now create the object
  auto opt = luaL_checkoption(
      L, 1, nullptr,
      (const char *const[]){sim::introspect_type<sim::object>::value,
                            sim::introspect_type<glview::view2d>::value,
                            sim::introspect_type<gui::text_ctrl>::value,
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

    if (lua_getfield(L, 2, "w") == LUA_TNUMBER) {
      auto w = lua_tointeger(L, -1);
      d_ptr->w = w;
    }
    lua_pop(L, 1);

    if (lua_getfield(L, 2, "h") == LUA_TNUMBER) {
      auto h = lua_tointeger(L, -1);
      d_ptr->h = h;
    }
    lua_pop(L, 1);

    d_ptr->font_dir = std::filesystem::path{session.root / "lib" / "fonts"};
    d_ptr->post_create();

    lua_pushinteger(L, idx);
    break;
  }
  case 2: {
    auto idx = session.create_object<gui::text_ctrl>();
    auto o_ptr = session.query_object(idx);
    auto d_ptr = dynamic_cast<gui::text_ctrl *>(o_ptr);
    // RECT
    if (lua_getfield(L, 2, "rect") == LUA_TTABLE) {
      if (lua_getfield(L, -1, "x") == LUA_TNUMBER) {
        d_ptr->rect->x = lua_tointeger(L, -1);
      }
      lua_pop(L, 1);

      if (lua_getfield(L, -1, "y") == LUA_TNUMBER) {
        d_ptr->rect->y = lua_tointeger(L, -1);
      }
      lua_pop(L, 1);
    }
    lua_pop(L, 1);

    // TEXT
    if (lua_getfield(L, 2, "text") == LUA_TSTRING) {
      auto text = lua_tostring(L, -1);
      d_ptr->text = text;
    }
    lua_pop(L, 1);

    // COLOR
    if (lua_getfield(L, 2, "color") == LUA_TTABLE) {
      if (lua_getfield(L, -1, "a") == LUA_TNUMBER) {
        d_ptr->color.a = lua_tointeger(L, -1);
      }
      lua_pop(L, 1);

      if (lua_getfield(L, -1, "b") == LUA_TNUMBER) {
        d_ptr->color.b = lua_tointeger(L, -1);
      }
      lua_pop(L, 1);

      if (lua_getfield(L, -1, "g") == LUA_TNUMBER) {
        d_ptr->color.g = lua_tointeger(L, -1);
      }
      lua_pop(L, 1);

      if (lua_getfield(L, -1, "r") == LUA_TNUMBER) {
        d_ptr->color.r = lua_tointeger(L, -1);
      }
      lua_pop(L, 1);
    }
    lua_pop(L, 1);

    // FONT
    if (lua_getfield(L, 2, "font") == LUA_TSTRING) {
      auto font = lua_tostring(L, -1);
      auto concated = std::filesystem::path{font, ".ttf"};
      auto path =
          std::filesystem::path{session.root / "lib" / "fonts" / concated};
      if (!std::filesystem::exists(path)) {
        lua_getglobal(L, "celerytest");
        lua_getfield(L, -1, "fallback_fontdir");
        auto fallback = lua_tostring(L, -1);
        path = std::filesystem::path{fallback / concated};
        lua_pop(L, 2);
      }
      d_ptr->font_path = path;
    }
    lua_pop(L, 1);

    d_ptr->post_create();

    lua_pushinteger(L, idx);
    break;
  }
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

[[maybe_unused]] int lua::remove(lua_State *L) {
  auto idx = luaL_checkinteger(L, 1);

  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));
  lua_pop(L, 2);
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
  lua_pop(L, 2);

  lua_pushinteger(L, session.sim_time);
  return 1;
}

[[maybe_unused]] int lua::render(lua_State *) {
  gl::tick();
  return 0;
}

[[maybe_unused]] int lua::poll(lua_State *L) {
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));
  lua_pop(L, 2);
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
      session.pending.pop();
      break;
    }
    }
  }
  lua_pushnil(L);
  return 1;
}
[[maybe_unused]] int lua::sleep(lua_State *L) {
  auto ms = luaL_checkinteger(L, 1);
  SDL_Delay(ms);
  return 0;
}
[[maybe_unused]] int lua::set_root_view(lua_State *L) {
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));
  lua_pop(L, 2);
  auto idx = luaL_checkinteger(L, 1);
  auto o_ptr = session.query_object(idx);

  if (o_ptr != nullptr) {
    if (o_ptr->get_type() == sim::types::glview_view2d) {
      auto r_ptr = dynamic_cast<glview::view2d *>(o_ptr);
      gl::set_root_view(&session, r_ptr);
    }
  }
  return 0;
}
[[maybe_unused]] int lua::get(lua_State *L) {
  auto o_ptr = quick_getobj(L);

  if (o_ptr != nullptr) {
    switch (o_ptr->get_type()) {
    case sim::types::glview_view2d: {
      auto opt = luaL_checkoption(L, 2, nullptr,
                                  (const char *const[]){"w", "h", nullptr});
      auto r_ptr = dynamic_cast<glview::view2d *>(o_ptr);
      switch (opt) {
      case 0: {
        lua_pushinteger(L, r_ptr->w);
        return 1;
      }
      case 1: {
        lua_pushinteger(L, r_ptr->h);
        return 1;
      }
      default: {
        break;
      }
      }
    }
    // NOTE: This is for a currently abstract-in-Lua, regular-in-C++ type.
    // Disabled in this build.
#if 0
    case sim::types::gui_ctrl: {
      auto opt = luaL_checkoption(
          L, 2, nullptr,
          (const char *const[]){"sw", "sh", "w", "h", "x", "y", nullptr});
      auto r_ptr = dynamic_cast<gui::ctrl *>(o_ptr);
      switch (opt) {
      case 0: {
        lua_pushinteger(L, r_ptr->surface->w);
        return 1;
      }
      case 1: {
        lua_pushinteger(L, r_ptr->surface->h);
        return 1;
      }
      case 2: {
        lua_pushinteger(L, r_ptr->rect->w);
        return 1;
      }
      case 3: {
        lua_pushinteger(L, r_ptr->rect->h);
        return 1;
      }
      case 4: {
        lua_pushinteger(L, r_ptr->rect->x);
        return 1;
      }
      case 5: {
        lua_pushinteger(L, r_ptr->rect->y);
        return 1;
      }
      default: {
        break;
      }
      }
    }
#endif
    case sim::types::gui_textctrl: {
      auto opt = luaL_checkoption(
          L, 2, nullptr,
          (const char *const[]){"rect", "text", "color", "font", nullptr});
      auto r_ptr = dynamic_cast<gui::text_ctrl *>(o_ptr);
      switch (opt) {
      case 0: {
        lua_newtable(L);
        lua_pushinteger(L, r_ptr->rect->w);
        lua_setfield(L, -2, "w");
        lua_pushinteger(L, r_ptr->rect->h);
        lua_setfield(L, -2, "h");
        lua_pushinteger(L, r_ptr->rect->x);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, r_ptr->rect->y);
        lua_setfield(L, -2, "y");
        return 1;
      }
      case 1: {
        lua_pushstring(L, r_ptr->text.c_str());
        return 1;
      }
      case 2: {
        lua_newtable(L);
        lua_pushinteger(L, r_ptr->color.a);
        lua_setfield(L, -2, "a");
        lua_pushinteger(L, r_ptr->color.b);
        lua_setfield(L, -2, "b");
        lua_pushinteger(L, r_ptr->color.g);
        lua_setfield(L, -2, "g");
        lua_pushinteger(L, r_ptr->color.r);
        lua_setfield(L, -2, "r");
        return 1;
      }
      case 3: {
        lua_pushstring(L, r_ptr->font_path.stem().c_str());
        return 1;
      }
      default: {
        break;
      }
      }
    }
    default: {
      break;
    }
    }
  }
  lua_pushnil(L);
  return 1;
}
[[maybe_unused]] int lua::set(lua_State *L) {
  auto o_ptr = quick_getobj(L);

  if (o_ptr != nullptr) {
    switch (o_ptr->get_type()) {
    case sim::types::gui_textctrl: {
      auto opt = luaL_checkoption(
          L, 2, nullptr,
          (const char *const[]){"rect", "text", "color", nullptr});
      auto r_ptr = dynamic_cast<gui::text_ctrl *>(o_ptr);
      switch (opt) {
      case 0: {
        // XPOS
        if (lua_getfield(L, 3, "x") == LUA_TNUMBER) {
          auto x = lua_tointeger(L, -1);
          r_ptr->rect->x = x;
        }
        lua_pop(L, 1);
        // YPOS
        if (lua_getfield(L, 3, "y") == LUA_TNUMBER) {
          auto y = lua_tointeger(L, -1);
          r_ptr->rect->y = y;
        }
        lua_pop(L, 1);
        break;
      }
      case 1: {
        auto text = luaL_checkstring(L, 3);
        r_ptr->text = text;
        r_ptr->dirty = true;
        break;
      }
      case 2: {
        // ALPHA
        if (lua_getfield(L, 3, "a") == LUA_TNUMBER) {
          auto a = lua_tointeger(L, -1);
          r_ptr->color.a = a;
        }
        lua_pop(L, 1);

        // BLUE
        if (lua_getfield(L, 3, "b") == LUA_TNUMBER) {
          auto b = lua_tointeger(L, -1);
          r_ptr->color.b = b;
        }
        lua_pop(L, 1);

        // GREEN
        if (lua_getfield(L, 3, "g") == LUA_TNUMBER) {
          auto g = lua_tointeger(L, -1);
          r_ptr->color.g = g;
        }
        lua_pop(L, 1);

        // RED
        if (lua_getfield(L, 3, "r") == LUA_TNUMBER) {
          auto r = lua_tointeger(L, -1);
          r_ptr->color.r = r;
        }
        lua_pop(L, 1);

        r_ptr->dirty = true;
        break;
      }
      default: {
        break;
      }
      }
    }
    default: {
      break;
    }
    }
  }
  return 0;
}
[[maybe_unused]] int lua::gui_insert(lua_State *L) {
  auto z = luaL_checkinteger(L, 1);
  auto idx = luaL_checkinteger(L, 2);
  gl::get_root_view()->ctrls_idx.assign(z, idx);
  return 0;
}
[[maybe_unused]] int lua::gui_objcnt(lua_State *L) {
  lua_pushinteger(L, gl::get_root_view()->ctrls_idx.size());
  return 1;
}
[[maybe_unused]] int lua::gui_remove(lua_State *L) {
  auto z = luaL_checkinteger(L, 1);
  if(z < 0) {
    gl::get_root_view()->ctrls_idx.erase(
        gl::get_root_view()->ctrls_idx.end() + z);
  } else {
    gl::get_root_view()->ctrls_idx.erase(
        gl::get_root_view()->ctrls_idx.begin() + z);
  }
  return 0;
}

//  quick utility functions
sim::object *lua::quick_getobj(lua_State *L) {
  lua_getglobal(L, "celerytest");
  lua_getfield(L, -1, "session");
  auto &&session = *reinterpret_cast<sim::session *>(lua_touserdata(L, -1));
  lua_pop(L, 2);
  auto idx = luaL_checkinteger(L, 1);
  return session.query_object(idx);
}
