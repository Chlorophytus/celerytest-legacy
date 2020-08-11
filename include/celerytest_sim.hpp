#include "celerytest.hpp"

namespace celerytest {
namespace sim {
enum class types : U16 {
  sim_object,
};
struct object {
  virtual types get_type() { return types::sim_object; }
};
template <typename T> struct is_object { constexpr static bool value = false; };
template <> struct is_object<object> { constexpr static bool value = true; };
// Store a bucket of objects. This makes it modular and easy to sieve.
// Also it allows us to allocate these objects lazier.
struct bucket {
  constexpr static size_t objects_per_bucket = 2 << 7;
  const bool full();
  std::bitset<objects_per_bucket> switchboard{false};
  std::array<std::unique_ptr<object>, objects_per_bucket> data{nullptr};
};
struct session {
  session();
  lua_State *L = nullptr;
  constexpr static size_t buckets_per_session = 2 << 15;
  template <typename T> const static size_t new_object() {
    static_assert(is_object<T>::value == true);
    auto bucket_offset = 0;
    for(auto &&bucket : data) {
      if(bucket) {

      }
    }
    return 0;
  }
  std::array<std::unique_ptr<bucket>, buckets_per_session> buckets{nullptr};
  ~session();
};
} // namespace sim
} // namespace celerytest
