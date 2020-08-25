#pragma once
// Sim (session) helpers
#include "celerytest.hpp"
#include "celerytest_con.hpp"

namespace celerytest {
namespace sim {
enum class types : U16 {
  sim_object,
  sim_context,
};
// OBJECTS
struct object {
  virtual types get_type() const { return types::sim_object; }
  virtual const char *get_type_string() const { return "SimObject"; }
};
template <typename T> struct is_object {
  constexpr const static bool value = false;
};
template <typename T> struct introspect_type {
  constexpr const static char *value = "Invalid";
};
template <> struct is_object<object> {
  constexpr const static bool value = true;
};
template <> struct introspect_type<object> {
  constexpr const static char *value = "SimObject";
};
// CONTEXTS
struct context : object {
  virtual types get_type() const { return types::sim_context; }
  virtual const char *get_type_string() const { return "SimContext"; }

};
template <> struct is_object<context> {
  constexpr const static bool value = true;
};
template <> struct introspect_type<context> {
  constexpr const static char *value = "SimContext";
};

// Store a bucket of objects. This makes it modular and easy to sieve.
// Also it allows us to allocate these objects lazier.
struct bucket {
  constexpr const static size_t objects_per_bucket = 1024;
  bool full() const;
  std::bitset<objects_per_bucket> switchboard{false};
  std::array<std::unique_ptr<object>, objects_per_bucket> data{};
};
struct session {
  [[maybe_unused]] session(const std::filesystem::path &, bool &&);
  lua_State *L = nullptr;
  constexpr const static size_t buckets_per_session = 2 << 9;
  const bool headless;

  // Create an object
  template <typename T> size_t create_object() {
    static_assert(is_object<T>::value == true);
    auto b_offset = 0;
    con::log_all(con::severity::debug,
                 {"creating a ", introspect_type<T>::value});
    for (auto &&b : buckets) {
      // Offset will start off with 0
      auto offset = 0;

      // Check if this bucket is present...
      if (b) {
        // We have a bucket here, check if it's full
        if (!b->full()) {
          // Check which slot is empty
          for (offset = 0; offset < bucket::objects_per_bucket; offset++) {
            if (!b->switchboard.test(offset)) {
              // This slot is empty, BREAK.
              break;
            }
          }
        } else {
          // It is full, continue.
          b_offset++;
          continue;
        }
      } else {
        // Great, we have no bucket here, so we *create* a bucket here.
        b = std::make_unique<bucket>();
      }

      con::log_all(con::severity::debug,
                   {"...at bucket ", std::to_string(b_offset), " offset ",
                    std::to_string(offset)});

      // Set this on the bucket's switchboard
      b->switchboard.set(offset);

      // Then create the object here.
      b->data.at(offset) = std::make_unique<T>();

      // Now return the strided object ID.
      return (b_offset * bucket::objects_per_bucket) + offset;
    }
    throw std::runtime_error{"all buckets full, can't create anymore objects"};
  }
  // Delete an object
  void delete_object(size_t);

  std::array<std::unique_ptr<bucket>, buckets_per_session> buckets{nullptr};
  ~session();
};
} // namespace sim
} // namespace celerytest
