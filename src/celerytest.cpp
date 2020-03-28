#include "../include/celerytest.hpp"
#include "../include/celerytest_interwork.hpp"
#include "../include/celerytest_log.hpp"
#include "../include/celerytest_lua.hpp"
#include "../include/celerytest_sim.hpp"

int main(int argc, char **argv) {
  celerytest::log(celerytest::severity::info,
                  {"celerytest ", celerytest_VSTRING_FULL});
  // If you don't specify the Width/Height then I will specify it for you.
  auto w = U16((argc == 4) ? std::atoi(argv[1]) : 0);
  auto h = U16((argc == 4) ? std::atoi(argv[2]) : 0);
  auto fullscreen = bool((argc == 4) ? std::atoi(argv[3]) != 0 : false);

  if (w == 0) {
    w = 640;
  }
  if (h == 0) {
    h = 480;
  }
  celerytest::log(
      celerytest::severity::info,
      {"using resolution ", std::to_string(w), "x", std::to_string(h)});

  if (fullscreen) {
    celerytest::log(celerytest::severity::info, {"fullscreen = true"});
  } else {
    celerytest::log(celerytest::severity::info, {"fullscreen = false"});
  }

  // SDL will return a non-zero value for failure in initialization.
  auto init = SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
  TTF_Init();
  if (init != 0) {
    celerytest::check_sdl_error();
  }
  assert(init == 0);
  celerytest::sim_init();
  auto inter = std::make_unique<celerytest::interwork>(w, h, fullscreen);
  #if 0
  auto e = 1;
  for (auto i = 0; i < 2 << 24; i++) {
    if (e == i) {
      celerytest::log(celerytest::severity::info, {"object creation timing ",
                      std::to_string(e), "..."});
      e <<= 1;
    }

    celerytest::sim_create_hint(1, i);
  }
  celerytest::log(celerytest::severity::info, {"object creation timing done"});
  #endif
  while (inter->tick())
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

  celerytest::sim_deinit();
  // Quit SDL lastly, **LASTLY**.
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  return 0;
}