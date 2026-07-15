#pragma once
#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using usize = size_t;
using isize = intptr_t;

using f32 = float;
using f64 = double;

#ifdef _WIN64
#define OS_WINDOWS 1
#define WIN_VER 0x0A00
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#endif

#ifdef __linux__
#define OS_LINUX 1
#endif
