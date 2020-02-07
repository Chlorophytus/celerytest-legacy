#pragma once
#include <SDL2/SDL.h>

// RATIONALE: In case version retrieval is needed
#include "sdl_template_cfg.hpp"

// NOTE: Assertions can be converted into exceptions, but assertions are simple.
#include <cassert>

// RATIONALE: Needed for printf, or you could use iostream instead
#include <cstdio>

// RATIONALE: Needed for our integer types that will be defined.
#include <cstdint>
using U8 = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

using S8 = std::int8_t;
using S16 = std::int16_t;
using S32 = std::int32_t;
using S64 = std::int64_t;

using F32 = float;
using F64 = double;

// RATIONALE: Needed for smart pointers
#include <memory>