#pragma once
// Sim (session) helpers
#include "celerytest.hpp"

namespace celerytest {
namespace sim {
enum class types : U16 {
  sim_object,
};
struct object {
  virtual types get_type() const { return types::sim_object; }
  virtual const char *get_type_string() const { return "SimObject"; }
};
template <typename T> struct is_object { constexpr const static bool value = false; };
template <typename T> struct introspect_type {
  constexpr const static char *value = "Invalid";
};
template <> struct is_object<object> { constexpr const static bool value = true; };
template <> struct introspect_type<object> {
  constexpr const static char *value = "SimObject";
};
// Store a bucket of objects. This makes it modular and easy to sieve.
// Also it allows us to allocate these objects lazier.
struct bucket {
  constexpr const static size_t objects_per_bucket = 2 << 7;
  const bool full();
  std::bitset<objects_per_bucket> switchboard{false};
  std::array<std::unique_ptr<object>, objects_per_bucket> data{nullptr};
};
struct session {
  session(const std::filesystem::path &, bool &&);
  lua_State *L = nullptr;
  constexpr const static size_t buckets_per_session = 2 << 15;
  const bool headless;

  // Create an object
  template <typename T> const size_t create_object() {
    static_assert(is_object<T>::value == true);
    auto b_offset = 0;
    con::log_all(con::severity::debug,
                 {"creating a ", introspect_type<T>::value});
    for (auto &&b : buckets) {
      if (b) {
        if (b->switchboard.all()) {
          b_offset++;
          continue;
        } else {
          assert(!b->full());
          auto offset = 0;
          for (; offset < bucket::objects_per_bucket; offset++) {
            if (!b->switchboard.test(offset)) {
              break;
            };
          }
          b->switchboard.set(offset);
          b->data.at(offset) = std::make_unique<T>();
          con::log_all(con::severity::debug,
                       {"...at bucket ", std::to_string(b_offset),
                        " offset ", std::to_string(offset)});
          return (b_offset * bucket::objects_per_bucket) + offset;
        }
      } else {
        b = std::make_unique<bucket>();
      }
    }
    assert(false);
  }
  // Delete an object
  void delete_object(const size_t);

  std::array<std::unique_ptr<bucket>, buckets_per_session> buckets{nullptr};
  ~session();
};
} // namespace sim
} // namespace celerytest
