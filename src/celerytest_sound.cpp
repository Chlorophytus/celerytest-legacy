#include "../include/celerytest_sound.hpp"
using namespace celerytest;

const auto pan_coeff = std::sqrt(2.0f) / 2.0f;
const auto root = std::filesystem::path(".");
auto engine = std::unique_ptr<celerytest::sound_engine>{nullptr};

void celerytest::sound_init() {
  engine = std::make_unique<celerytest::sound_engine>();
  auto loader_fd = std::fopen((root / "lib" / "CGESound1.bin").c_str(), "r");
  assert(loader_fd);
  // Load sound program
  for (auto &&byte : engine->program_memory) {
    byte = std::fgetc(loader_fd);
  }
  // Clear echo buffers
  for (auto &&byte : engine->echobuffer_l) {
    byte = 0;
  }
  for (auto &&byte : engine->echobuffer_r) {
    byte = 0;
  }
  std::fclose(loader_fd);
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

F32 sound_engine::square(F32 hertz, F32 duty, U8 placement) {
  auto t = static_cast<F32>(ticks[placement]++) / 11025.0f;
  if (fmod(t * hertz, 1.0f) > duty) {
    return 1.0f;
  } else {
    return -1.0f;
  }
}

void sound_engine::ldi_gpr(U8 which) {
  program_counter++; // increment so we can load the word
  auto where = program_memory[program_counter];
  gprs[which] = program_memory[where + 1];
  gprs[which] <<= 8;
  gprs[which] |= program_memory[where + 0];
  program_counter++;
}

void sound_engine::lda_gpr(U8 which) {
  program_counter++; // increment so we can load the address
  auto where = U16{program_memory[program_counter]};
  program_counter++;
  where <<= 8;
  where |= program_memory[program_counter];
  gprs[which] = program_memory[where + 1];
  gprs[which] <<= 8;
  gprs[which] |= program_memory[where + 0];
  program_counter++;
}

void sound_engine::sti_gpr(U8 which) {
  program_counter++;
  auto where = program_memory[program_counter];
  program_memory[where + 1] = (gprs[which] & 0xFF00) >> 8;
  program_memory[where + 0] = (gprs[which] & 0x00FF) >> 0;
  program_counter++;
}

void sound_engine::sta_gpr(U8 which) {
  program_counter++; // increment so we can load the address
  auto where = U16{program_memory[program_counter]};
  program_counter++;
  where <<= 8;
  where |= program_memory[program_counter];
  program_memory[where + 1] = (gprs[which] & 0xFF00) >> 8;
  program_memory[where + 0] = (gprs[which] & 0x00FF) >> 0;
  program_counter++;
}

void sound_engine::alui_gprs(alu_ops what) {
  program_counter++;
  auto out = program_memory[program_counter] & 0x0F;
  program_counter++; // increment so we can load the value
  auto in = U16{program_memory[program_counter]};
  program_counter++;
  in <<= 8;
  in |= program_memory[program_counter];
  program_counter++;
  switch (what) {
  case alu_ops::oadd: {
    gprs[out] += in;
    carry = gprs[out] < in;
    break;
  }
  case alu_ops::osub: {
    gprs[out] -= in;
    carry = gprs[out] > in;
    break;
  }
  case alu_ops::omul: {
    gprs[out] *= in;
    carry = gprs[out] < in;
    break;
  }
  case alu_ops::oshr: {
    gprs[out] >>= in;
    break;
  }
  case alu_ops::oand: {
    gprs[out] &= in;
    break;
  }
  case alu_ops::oor: {
    gprs[out] |= in;
    break;
  }
  case alu_ops::oshl: {
    gprs[out] <<= in;
    break;
  }
  case alu_ops::oxor: {
    gprs[out] ^= in;
    break;
  }
  }
  zero = gprs[out] == 0;
}

void sound_engine::alur_gprs(alu_ops what) {
  program_counter++;
  auto which = program_memory[program_counter];
  program_counter++;
  auto in = (which & 0xF0) >> 4;
  auto out = (which & 0x0F) >> 0;
  switch (what) {
  case alu_ops::oadd: {
    gprs[out] += gprs[in];
    carry = gprs[out] < gprs[in];
    break;
  }
  case alu_ops::osub: {
    gprs[out] -= gprs[in];
    carry = gprs[out] > gprs[in];
    break;
  }
  case alu_ops::omul: {
    gprs[out] *= gprs[in];
    carry = gprs[out] < gprs[in];
    break;
  }
  case alu_ops::oshr: {
    gprs[out] >>= gprs[in];
    break;
  }
  case alu_ops::oand: {
    gprs[out] &= gprs[in];
    break;
  }
  case alu_ops::oor: {
    gprs[out] |= gprs[in];
    break;
  }
  case alu_ops::oshl: {
    gprs[out] <<= gprs[in];
    break;
  }
  case alu_ops::oxor: {
    gprs[out] ^= gprs[in];
    break;
  }
  }
  zero = gprs[out] == 0;
}
void sound_engine::ldc_gpr(U8 what) {
  gprs[what] = clock;
  zero = gprs[what] == 0;
  program_counter++;
}
void sound_engine::manipulate_flag(branch_ops what, bool val) {
  if (what == branch_ops::ocarry)
    carry = val;
  if (what == branch_ops::ozero)
    zero = val;
  program_counter++;
}
void sound_engine::branch_if(branch_ops condition) {
  program_counter++; // increment so we can load the value
  auto in = U16{program_memory[program_counter]};
  program_counter++;
  in <<= 8;
  in |= program_memory[program_counter];
  program_counter++;
  switch (condition) {
  case branch_ops::oalways: {
    program_counter = in;
    break;
  }
  case branch_ops::onever: {
    program_counter++;
    break;
  }
  case branch_ops::ozero: {
    if (zero) {
      program_counter = in;
    } else {
      program_counter++;
    }
    break;
  }
  case branch_ops::ocarry: {
    if (carry) {
      program_counter = in;
    } else {
      program_counter++;
    }
    break;
  }
  }
}
void sound_engine::calc_pan(F32 &l, F32 &r, F32 pan) {
  // this is tricky sticky
  auto lp = pan_coeff * (std::cos(pan) + std::sin(pan));
  auto rp = pan_coeff * (std::cos(pan) - std::sin(pan));
  l *= lp;
  r *= rp;
}
void sound_engine::mix(U8 &l, U8 &r) {
  for (clock = 0x00; clock < 0xFF; clock++) {
    switch (program_memory[program_counter]) {
    case 0x00: { // Break
      break;
    }
    case 0x10: { // Load Zero Page Immediate to R0
      ldi_gpr(0);
      break;
    }
    case 0x11: { // ... R1
      ldi_gpr(1);
      break;
    }
    case 0x12: { // ... R2
      ldi_gpr(2);
      break;
    }
    case 0x13: { // ... R3
      ldi_gpr(3);
      break;
    }
    case 0x14: { // Load Address to R0
      lda_gpr(0);
      break;
    }
    case 0x15: { // ... R1
      lda_gpr(1);
      break;
    }
    case 0x16: { // ... R2
      lda_gpr(2);
      break;
    }
    case 0x17: { // ... R3
      lda_gpr(3);
      break;
    }
    case 0x20: { // Store Zero Page Immediate From R0
      sti_gpr(0);
      break;
    }
    case 0x21: { // ... R1
      sti_gpr(1);
      break;
    }
    case 0x22: { // ... R2
      sti_gpr(2);
      break;
    }
    case 0x23: { // ... R3
      sti_gpr(3);
      break;
    }
    case 0x24: { // Store Address From R0
      sta_gpr(0);
      break;
    }
    case 0x25: { // ... R1
      sta_gpr(1);
      break;
    }
    case 0x26: { // ... R2
      sta_gpr(2);
      break;
    }
    case 0x27: { // ... R3
      sta_gpr(3);
      break;
    }
    case 0x30: {
      alur_gprs(alu_ops::oadd);
      break;
    }
    case 0x31: {
      alur_gprs(alu_ops::osub);
      break;
    }
    case 0x32: {
      alur_gprs(alu_ops::omul);
      break;
    }
    case 0x33: {
      alur_gprs(alu_ops::oshl);
      break;
    }
    case 0x34: {
      alur_gprs(alu_ops::oshr);
      break;
    }
    case 0x35: {
      alur_gprs(alu_ops::oand);
      break;
    }
    case 0x36: {
      alur_gprs(alu_ops::oor);
      break;
    }
    case 0x37: {
      alur_gprs(alu_ops::oxor);
      break;
    }
    case 0x38: {
      alur_gprs(alu_ops::oadd);
      break;
    }
    case 0x39: {
      alur_gprs(alu_ops::osub);
      break;
    }
    case 0x3A: {
      alur_gprs(alu_ops::omul);
      break;
    }
    case 0x3B: {
      alur_gprs(alu_ops::oshl);
      break;
    }
    case 0x3C: {
      alur_gprs(alu_ops::oshr);
      break;
    }
    case 0x3D: {
      alur_gprs(alu_ops::oand);
      break;
    }
    case 0x3E: {
      alur_gprs(alu_ops::oor);
      break;
    }
    case 0x3F: {
      alur_gprs(alu_ops::oxor);
      break;
    }
    case 0x40: { // load clock
      ldc_gpr(0);
      break;
    }
    case 0x41: {
      ldc_gpr(1);
      break;
    }
    case 0x42: {
      ldc_gpr(2);
      break;
    }
    case 0x43: {
      ldc_gpr(3);
      break;
    }
    case 0x44: {
      manipulate_flag(branch_ops::ozero, false);
      break;
    }
    case 0x45: {
      manipulate_flag(branch_ops::ocarry, false);
      break;
    }
    case 0x46: {
      manipulate_flag(branch_ops::ozero, true);
      break;
    }
    case 0x47: {
      manipulate_flag(branch_ops::ocarry, true);
      break;
    }
    case 0x48: {
      branch_if(branch_ops::oalways);
      break;
    }
    case 0x49: {
      branch_if(branch_ops::onever);
      break;
    }
    case 0x4A: {
      branch_if(branch_ops::ozero);
      break;
    }
    case 0x4B: {
      branch_if(branch_ops::ocarry);
      break;
    }
    default: {
      program_counter++;
      break;
    }
    }
  } // repeat a few times then do sound rendering
  for (auto i = 0; i < 4; i++) {
    auto tuning = U16{program_memory[i * 32 + 0]};
    tuning <<= 8;
    tuning |= program_memory[i * 32 + 1];
    auto duty = program_memory[i * 32 + 2];
    auto pan = program_memory[i * 32 + 3];
    auto amplitude = program_memory[i * 32 + 4];
    auto key = program_memory[i * 32 + 5];
    auto sl = 0.0f;
    auto sr = 0.0f;
    if (key != 0) {
      auto true_tuning = static_cast<F32>(tuning) / 32.0f;
      auto true_amplitude = static_cast<F32>(amplitude) / 256.0f;
      auto true_duty = static_cast<F32>(duty) / 256.0f;
      auto true_pan = ((static_cast<F32>(pan) - 128.0f) / 256.0f) * 45.0f;
      // printf("%hhd %fHz %fA %f%%D %fPAN\n", i, true_tuning, true_amplitude,
      //        true_duty, true_pan);
      auto sound = square(true_tuning, true_duty, i) * true_amplitude;
      sl = sound / 4.0f;
      sr = sound / 4.0f;
      calc_pan(sl, sr, true_pan);
    }
    // calculate left channel
    echobuffer_l[echo_ptr_l] += static_cast<U8>((sl * 64.0f) + 127.0f);
    echobuffer_l[echo_ptr_l] *=
        (static_cast<F32>(program_memory[130]) + 1.0f) / 512.0f;
    // now the right channel
    echobuffer_r[echo_ptr_r] += static_cast<U8>((sr * 64.0f) + 127.0f);
    echobuffer_r[echo_ptr_r] *=
        (static_cast<F32>(program_memory[131]) + 1.0f) / 512.0f;
    l += sl - echobuffer_l[echo_ptr_l];
    r += sr - echobuffer_r[echo_ptr_r];
    //  printf("%hhxL %hhxR\n", echobuffer_l[echo_ptr_l], echobuffer_r[echo_ptr_r]);
  }
  echo_ptr_l++;
  echo_ptr_l %= U16{program_memory[128]} << 8;
  echo_ptr_r++;
  echo_ptr_r %= U16{program_memory[129]} << 8;
}

void celerytest::sound_deinit() { SDL_CloseAudio(); }
