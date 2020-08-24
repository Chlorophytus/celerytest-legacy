#include "../include/celerytest_con.hpp"

#include "../include/celerytest_sim.hpp"

using namespace celerytest;

sim::session::session(const std::filesystem::path &root, bool &&headless)
    : headless{headless} {
  con::log_all(con::severity::debug, {"creating a session"});
  L = luaL_newstate();
  luaL_openlibs(L);
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

void sim::session::delete_object(const size_t idx) {
  auto b_offset = idx / sim::bucket::objects_per_bucket;
  auto offset = idx % sim::bucket::objects_per_bucket;
  con::log_all(con::severity::debug,
               {"deleting object ", std::to_string(offset), " at bucket ",
                std::to_string(b_offset)});
  auto &&b = buckets.at(b_offset);
  if (b->switchboard.test(offset)) {
    b->data.at(offset) = nullptr;
    b->switchboard.reset(offset);
  }
}