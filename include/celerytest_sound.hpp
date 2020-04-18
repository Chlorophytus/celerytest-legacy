#pragma once
#include "celerytest.hpp"

namespace celerytest {
struct sound_engine {
  enum class alu_ops : U8 { oadd, osub, omul, oshl, oshr, oand, oor, oxor };
  enum class branch_ops : U8 {
    oalways,
    onever,
    ozero,
    ocarry,
  };
  bool carry{false};
  bool zero{true};
  U16 program_counter{0xFFFC};                 // general program counter
  U8 clock{0x00};                              // where are we?
  U16 gprs[4]{0x0000, 0x0000, 0x0000, 0x0000}; // general program registers
  U16 echo_ptr_l{0};                           // echobuffer counter L
  U16 echo_ptr_r{0};                           // echobuffer counter R
  // U16 echo_boundary_l{0x1000};                      // echobuffer limit L
  // U16 echo_boundary_r{0x1000};                      // echobuffer limit R
  // U8 echo_attenuation_l{255};              // echo attenuation L divide by 255
  // U8 echo_attenuation_r{255};              // echo attenuation R divide by 255
  U8 program_memory[65536];                // program memory
  U8 echobuffer_l[65536];                  // echobuffer L
  U8 echobuffer_r[65536];                  // echobuffer R
  // U16 freq[4]{0, 0, 0, 0};                 // frequency
  // U8 duty[4]{0x00, 0x00, 0x00, 0x00};      // oscillator duty divide by 255
  // U8 amplitude[4]{0x00, 0x00, 0x00, 0x00}; // oscillator amplitude
  // U8 pan[4]{0x00, 0x00, 0x00, 0x00};       // oscillator panning
  U16 ticks[4]{0, 0, 0, 0};                // how many ticks for osc
  F32 square(F32, F32, U8);
  void calc_pan(F32 &, F32 &, F32);
  void ldi_gpr(U8);
  void lda_gpr(U8);
  void ldc_gpr(U8);
  void sti_gpr(U8);
  void sta_gpr(U8);
  void branch_if(branch_ops);
  void manipulate_flag(branch_ops, bool);
  void alur_gprs(alu_ops);
  void alui_gprs(alu_ops);
  void mix(U8 &, U8 &);
};
void sound_init();
void sound_callback(void *, Uint8 *, int);
void sound_deinit();
} // namespace celerytest