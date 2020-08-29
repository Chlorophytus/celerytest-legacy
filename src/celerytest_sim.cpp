#include "../include/celerytest_sim.hpp"
#include "../include/celerytest_con.hpp"
#include "../include/celerytest_gl.hpp"
#include "../include/celerytest_lua.hpp"

using namespace celerytest;

bool sim::bucket::full() const { return switchboard.all(); }

[[maybe_unused]] sim::session::session(const std::filesystem::path &root,
                                       bool &&headless)
    : headless{headless}, buckets{} {
  con::log_all(con::severity::debug, {"creating a session"});
  for (auto &&bucket : buckets) {
    bucket = nullptr;
  }
  L = luaL_newstate();
  luaL_openlibs(L);

  // `Celerytest` TABLE BELOW
  lua_newtable(L); // create table to -1

  lua_pushstring(L, "session");
  lua_pushlightuserdata(L, this);
  lua_settable(L, -3);

  // SET FUNCTIONS BELOW

  lua::set_one_function(L, "log", lua::log);
  lua::set_one_function(L, "create", lua::create);
  lua::set_one_function(L, "create_glcontext", lua::create_glcontext);
  lua::set_one_function(L, "remove_glcontext", lua::remove_glcontext);
  lua::set_one_function(L, "get_sim_time", lua::get_sim_time);
  lua::set_one_function(L, "set_root_view", lua::set_root_view);
  lua::set_one_function(L, "poll", lua::poll);
  lua::set_one_function(L, "render", lua::render);
  lua::set_one_function(L, "remove", lua::remove);
  lua::set_one_function(L, "sleep", lua::sleep);

  // SET FUNCTIONS ABOVE

  lua_setglobal(L, "celerytest");
  // `Celerytest` TABLE ABOVE

  if (headless) {
    auto e = luaL_dofile(L, (root / "init_server.lua").c_str());
    if (e) {
      con::log_all(con::severity::error,
                   {"`init_server.lua` failure: ", lua_tostring(L, -1)});
      lua_pop(L, 1);
    }
  } else {
    auto e = luaL_dofile(L, (root / "init_client.lua").c_str());
    if (e) {
      con::log_all(con::severity::error,
                   {"`init_client.lua` failure: ", lua_tostring(L, -1)});
      lua_pop(L, 1);
    }
  }
}
sim::session::~session() {
  lua_close(L);
  con::log_all(con::severity::debug, {"deleting a session"});
}

void sim::session::delete_object(size_t idx) {
  auto b_offset = idx / sim::bucket::objects_per_bucket;
  auto offset = idx % sim::bucket::objects_per_bucket;
  con::log_all(con::severity::debug,
               {"trying to delete object ", std::to_string(offset),
                " at bucket ", std::to_string(b_offset)});
  auto &&b = buckets.at(b_offset);
  if (b) {
    if (b->switchboard.test(offset)) {
      b->data.at(offset) = nullptr;
      b->switchboard.reset(offset);
      con::log_all(con::severity::debug, {"...deletion success"});
    }
  }
}

void sim::session::tick() {
  SDL_Event *ev;
  do {
    ev = new SDL_Event;
    pending.emplace(
        (sim::event){.data = std::unique_ptr<SDL_Event>{ev}, .time = sim_time});
    con::log_all(con::severity::debug, {"event, please check queue (tick #",
                                        std::to_string(sim_time), ")"});
  } while (SDL_PollEvent(ev));
  sim_time++;
}
sim::object *sim::session::query_object(size_t idx) {
  auto b_offset = idx / sim::bucket::objects_per_bucket;
  auto offset = idx % sim::bucket::objects_per_bucket;
  auto &&b = buckets.at(b_offset);
  if (b) {
    return b->data.at(offset).get();
  } else {
    return nullptr;
  }
}
