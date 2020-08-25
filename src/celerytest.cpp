#include "../include/celerytest.hpp"
#include "../include/celerytest_con.hpp"
#include "../include/celerytest_sim.hpp"

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  auto return_code = EXIT_FAILURE;
  auto root = std::filesystem::path{celerytest_SDIR};
  celerytest::con::init();
  auto console = new celerytest::con::file_listener{root / "console.log"};
  auto out = new celerytest::con::stdout_listener{};
  celerytest::con::attach(console);
  celerytest::con::attach(out);
  celerytest::con::log_all(celerytest::con::severity::informational,
                           {"celerytest ", celerytest_VSTRING_FULL});
  celerytest::con::log_all(
      celerytest::con::severity::informational,
      {"configured with ",
       std::to_string(celerytest::sim::bucket::objects_per_bucket),
       " objects/bucket"});
  celerytest::con::log_all(
      celerytest::con::severity::informational,
      {"configured with ",
       std::to_string(celerytest::sim::session::buckets_per_session),
       " buckets/session"});

  celerytest::con::log_all(
      celerytest::con::severity::informational,
      {"configured with ",
       std::to_string(celerytest::sim::bucket::objects_per_bucket *
                      celerytest::sim::session::buckets_per_session),
       " objects/session"});
  try {
    auto session = new celerytest::sim::session{root / "lua", false};

    // Hardcoded session code goes here.

    delete session;
    return_code = EXIT_SUCCESS;
  } catch (std::exception &e) {
    celerytest::con::log_all(celerytest::con::severity::emergency,
                             {"**Uncaught Exception**: ", e.what()});
  }
  celerytest::con::detach(0);
  celerytest::con::detach(0);
  celerytest::con::deinit();
  delete out;
  delete console;
  SDL_Quit();
  return return_code;
}
