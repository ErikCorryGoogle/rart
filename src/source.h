// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_SOURCE_H_
#define SRC_SOURCE_H_

#include "src/list_builder.h"
#include "src/zone.h"

namespace rart {

class Location {
  static const u32 kInvalid = 0xFFFFFFFF;

 public:
  Location() : value_(kInvalid) {
  }

  Location operator+(u32 offset) {
    return Location(value_ + offset);
  }

  u32 raw() const { return value_; }

  bool IsInvalid() const { return value_ == kInvalid; }

 private:
  explicit Location(u32 value) : value_(value) {
  }

  u32 value_;

  friend class Source;
};

class Source : public StackAllocated {
  static const int kChunkBits = 12;
  static const int kChunkSize = 1 << kChunkBits;

 public:
  explicit Source(Zone* zone);

  Location LoadFile(const char* path);
  Location LoadFromBuffer(const char* path, const char* source, u32 size);

  const char* GetSource(Location location);
  const char* GetFilePath(Location location);
  const char* GetLine(Location location, int* line_length);

 private:
  class Chunk {
   public:
    const char* file_path;
    const char* file_start;
    u32 chunk_offset;
  };

  ListBuilder<Chunk, 8> chunks_;
};

}  // namespace rart

#endif  // SRC_SOURCE_H_
