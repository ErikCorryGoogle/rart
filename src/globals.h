// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_GLOBALS_H_
#define SRC_GLOBALS_H_

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

// Types for native machine words. Guaranteed to be able to hold
// pointers and integers.
typedef long word;  // NOLINT
typedef unsigned long uword;  // NOLINT

// Introduce integer types with specific bit widths.
typedef signed char i8;
typedef short i16;  // NOLINT
typedef int i32;

typedef unsigned char u8;
typedef unsigned short u16;  // NOLINT
typedef unsigned int u32;

#ifdef FLETCH64
typedef long i64;  // NOLINT
typedef unsigned long u64;  // NOLINT
#else
typedef long long int i64;  // NOLINT
typedef long long unsigned u64;  // NOLINT
#endif

// Byte sizes.
const int kWordSize = sizeof(word);
const int kDoubleSize = sizeof(double);  // NOLINT
const int kPointerSize = sizeof(void*);  // NOLINT

#ifdef FLETCH64
const int kPointerSizeLog2 = 3;
const int kAlternativePointerSize = 4;
#else
const int kPointerSizeLog2 = 2;
const int kAlternativePointerSize = 8;
#endif

// Bit sizes.
const int kBitsPerByte = 8;
const int kBitsPerByteLog2 = 3;
const int kBitsPerPointer = kPointerSize * kBitsPerByte;
const int kBitsPerWord = kWordSize * kBitsPerByte;

// System-wide named constants.
const int KB = 1024;
const int MB = KB * KB;
const int GB = KB * KB * KB;

#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "Only little endian hosts are supported"
#endif

// A macro to disallow the copy constructor and operator= functions.
// This should be used in the private: declarations for a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// A macro to disallow all the implicit constructors, namely the default
// constructor, copy constructor and operator= functions. This should be
// used in the private: declarations for a class that wants to prevent
// anyone from instantiating it. This is especially useful for classes
// containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                                    \
  DISALLOW_COPY_AND_ASSIGN(TypeName)

// Macro to disallow allocation in the C++ heap. This should be used
// in the private section for a class.
#define DISALLOW_ALLOCATION()         \
  void* operator new(size_t size);    \
  void operator delete(void* pointer)

// The expression ARRAY_SIZE(array) is a compile-time constant of type
// size_t which represents the number of elements of the given
// array. You should only use ARRAY_SIZE on statically allocated
// arrays.
#define ARRAY_SIZE(array)                                   \
  ((sizeof(array) / sizeof(*(array))) /                     \
  static_cast<size_t>(!(sizeof(array) % sizeof(*(array)))))

// The expression OFFSET_OF(type, field) computes the byte-offset of
// the specified field relative to the containing type. This doesn't
// use 0 or NULL, which causes a problem with the compiler warnings we
// have enabled (which is also why 'offsetof' doesn't seem to work).
// The workaround is to use the non-zero value kDoubleSize.
#define OFFSET_OF(type, field) \
  (reinterpret_cast<uword>(&(reinterpret_cast<type*>(kDoubleSize)->field)) - \
     kDoubleSize)

// The USE(x) template is used to silence C++ compiler warnings issued
// for unused variables.
template <typename T>
static inline void USE(T) { }

// The bit_cast function uses the memcpy exception to move the bits
// from a variable of one type to a variable of another type. Of
// course the end result is likely to be implementation dependent.
// Most compilers (gcc-4.2 and MSVC 2005) will completely optimize
// bit_cast away.
//
// Do not use this to get around strict aliasing, use -fno-strict-aliasing
// instead.
template <class D, class S>
inline D bit_cast(const S& source) {
  // Compile time assertion: sizeof(D) == sizeof(S). A compile error
  // here means your D and S have different sizes.
  char VerifySizesAreEqual[sizeof(D) == sizeof(S) ? 1 : -1];
  USE(VerifySizesAreEqual);

  D destination;
  memcpy(&destination, &source, sizeof(destination));
  return destination;
}

#ifdef TEMP_FAILURE_RETRY
#undef TEMP_FAILURE_RETRY
#endif
// The definition below is copied from Linux and adapted to avoid lint
// errors (type long int changed to intptr_t and do/while split on
// separate lines with body in {}s) and to also block signals.
#define TEMP_FAILURE_RETRY(expression)                                         \
    ({ intptr_t __result;                                                      \
       do {                                                                    \
         __result = (expression);                                              \
       } while ((__result == -1L) && (errno == EINTR));                        \
       __result; })

#define VOID_TEMP_FAILURE_RETRY(expression)                                    \
    (static_cast<void>(TEMP_FAILURE_RETRY(expression)))

#endif  // SRC_GLOBALS_H_
