// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "src/utils.h"

namespace rart {

u32 Utils::StringHash(const uint16_t* data, int length) {
  // This implementation is based on the public domain MurmurHash
  // version 2.0. It assumes that we the underlying CPU can read from
  // unaligned addresses. The constants M and R have been determined
  // to work well experimentally.
  const u32 M = 0x5bd1e995;
  const int R = 24;
  int size = length * sizeof(uint16_t);
  u32 hash = size;

  // Mix four bytes at a time into the hash.
  const u8* cursor = reinterpret_cast<const u8*>(data);
  while (size >= 4) {
    u32 part = *reinterpret_cast<const u32*>(cursor);
    part *= M;
    part ^= part >> R;
    part *= M;
    hash *= M;
    hash ^= part;
    cursor += 4;
    size -= 4;
  }

  // Handle the last few bytes of the string.
  switch (size) {
    case 3:
      hash ^= cursor[2] << 16;
    case 2:
      hash ^= cursor[1] << 8;
    case 1:
      hash ^= cursor[0];
      hash *= M;
  }

  // Do a few final mixes of the hash to ensure the last few bytes are
  // well-incorporated.
  hash ^= hash >> 13;
  hash *= M;
  hash ^= hash >> 15;
  return hash;
}

}  // namespace rart
