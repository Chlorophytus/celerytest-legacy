#include "../include/celerytest_sound.hpp"
using namespace celerytest;

const auto pan_coeff = std::sqrt(2.0f) / 2.0f;
const auto root = std::filesystem::path(".");
auto engine = std::unique_ptr<celerytest::sound_engine>{nullptr};

void celerytest::sound_init() {
  engine = std::make_unique<celerytest::sound_engine>();
  auto loader_fd = std::fopen((root / "lib" / "CGESound1.bin").c_str(), "r");
  SDL_AudioSpec want, have;
  SDL_memset(&want, 0, sizeof(want));
  want.freq = 11025;
  want.format = AUDIO_U8;
  want.channels = 2;
  want.samples = 4096;
  want.callback = &celerytest::sound_callback;
  want.userdata = engine.get();
  SDL_OpenAudio(&want, &have);
  SDL_PauseAudio(0);
}

void celerytest::sound_callback(void *userdata, U8 *stream, int len) {
  auto casted = reinterpret_cast<celerytest::sound_engine *>(userdata);
  for (auto i = 0; i < len; i += 2) {
    casted->mix(stream[i + 0], stream[i + 1]);
  }
}

void celerytest::sound_deinit() { SDL_CloseAudio(); }

void sound_engine::mix(U8 &L, U8 &R) {
  L = 127;
  R = 127;
}