#include "../include/celerytest.hpp"
#include "../include/celerytest_con.hpp"
#include "../include/celerytest_sim.hpp"

int main(int argc, char **argv) {
#ifndef WIN32
  __builtin_cpu_init();
#endif
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();

  auto return_code = EXIT_FAILURE;
  auto root = std::filesystem::current_path();
  celerytest::con::init();
  auto console = new celerytest::con::file_listener{root / "console.log"};
  auto out = new celerytest::con::stdout_listener{};
  celerytest::con::attach(console);
  celerytest::con::attach(out);
  celerytest::con::log_all(celerytest::con::severity::informational,
                           std::string{"celerytest "} + std::string{celerytest_VSTRING_FULL});
  celerytest::con::log_all(celerytest::con::severity::informational,
                           std::string{"path is '"} + root.string() +
                               std::string{"'"});
  celerytest::con::log_all(
      celerytest::con::severity::informational,
      std::string{"configured with "} +
          std::to_string(celerytest::sim::bucket::objects_per_bucket) +
          std::string{" objects/bucket"});
  celerytest::con::log_all(
      celerytest::con::severity::informational,
      std::string{"configured with "} +
          std::to_string(celerytest::sim::session::buckets_per_session) +
          std::string{" buckets/session"});

  celerytest::con::log_all(
      celerytest::con::severity::informational,
      std::string{"configured with "} +
          std::to_string(celerytest::sim::bucket::objects_per_bucket *
                         celerytest::sim::session::buckets_per_session) +
          std::string{" objects/session"});
  celerytest::con::log_cpuid();
  try {
    auto session = new celerytest::sim::session{root, false};

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

  TTF_Quit();
  SDL_Quit();
  return return_code;
}
