#pragma once

#define U32_MAX 0xfffffffful
#define U64_MAX 0xffffffffffffffffull

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;

typedef signed long long I64;
typedef signed int I32;
typedef signed short I16;
typedef signed char I8;

typedef double F64;
typedef float F32;

struct VkExtent2D;

namespace Onyx {
//! Struct that defines a 2D area.
struct Extent2D {
  U32 Width;   //!< Width of the 2D area.
  U32 Height;  //!< Height of the 2D area.

  operator VkExtent2D();
};
}  // namespace Onyx