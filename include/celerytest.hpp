#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <lua.hpp>

// RATIONALE: In case version retrieval is needed
#include "celerytest_cfg.hpp"

// NOTE: Assertions can be converted into exceptions, but assertions are
// simple.
#include <cassert>

#include <bitset>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <forward_list>
#include <initializer_list>
#include <memory>
#include <string_view>

// NOTE: Unordered maps are really fast.
#include <unordered_map>

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
