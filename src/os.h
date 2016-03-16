// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_OS_H_
#define SRC_OS_H_

#include "src/globals.h"
#include "src/list.h"

namespace rart {

class Zone;

class OS {
 public:
  static i64 CurrentTime();

  // Resolve 'path' relative to 'uri'.
  // If 'zone' is NULL, malloc is used for allocating the buffer.
  static const char* UriResolve(const char* uri, const char* path, Zone* zone);

  // Load file at 'uri'. The returned buffer is '0' terminated.
  // If 'zone' is NULL, malloc is used for allocating the buffer.
  static char* LoadFile(const char* uri,
                        Zone* zone,
                        u32* file_size = NULL);

  // Store file at 'uri'.
  static bool StoreFile(const char* uri, List<u8> bytes);
};

}  // namespace rart

#endif  // SRC_OS_H_
