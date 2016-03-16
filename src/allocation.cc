// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "src/allocation.h"
#include "src/zone.h"

namespace rart {

StackResource::StackResource() {
}

StackResource::~StackResource() {
}

void* ZoneAllocated::operator new(size_t size, Zone* zone) {
  return zone->Allocate(size);
}

}  // namespace rart
