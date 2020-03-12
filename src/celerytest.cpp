#include "../include/celerytest.hpp"
#include "../include/celerytest_interwork.hpp"
#include "../include/celerytest_lua.hpp"

int main(int argc, char **argv) {
  std::printf("celerytest %s\n", celerytest_VSTRING_FULL);

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

  std::printf("using resolution %hux%hu\n", w, h);

  if (fullscreen) {
    std::printf("fullscreen = true\n");
  } else {
    std::printf("fullscreen = false\n");
  }

  // SDL will return a non-zero value for failure in initialization.
  auto init = SDL_Init(SDL_INIT_EVERYTHING);
  if(init != 0) {
    celerytest::check_sdl_error();
  }
  assert(init == 0);
  auto inter = std::make_unique<celerytest::interwork>(w, h, fullscreen);

  while (inter->tick())
    SDL_Delay(10);

  // Quit SDL lastly, **LASTLY**.
  SDL_Quit();
  return 0;
}