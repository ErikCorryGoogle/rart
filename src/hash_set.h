// Copyright (c) 2015, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_VM_HASH_SET_H_
#define SRC_VM_HASH_SET_H_

#include "src/hash_table.h"
#include "src/zone.h"

namespace rart {

template <typename Key>
struct SetKeyExtractor {
  static Key& GetKey(Key& key) {  // NOLINT
    return key;
  }

  static const Key& GetKey(const Key& key) { return key; }
};

// UnorderedSet:
// Interface is kept as close as possible to std::unordered_map, but:
// * Only functions that can be expected to be tiny are inlined.
// * The Key type must be memcpy-able and castable to void* and the same size.
// * The hash code is just a cast to int.
// * Not all methods are implemented.
// * Iterators are invalidated on all inserts, even if the key was already
//   present.
// * Google naming conventions are used (CamelCase classes and methods).
template <typename Key>
class HashSet : public UnorderedHashTable<Key, Key, SetKeyExtractor<Key>> {
 public:
  explicit HashSet(Zone* zone) : UnorderedHashTable(zone) {}

 private:
  typedef UnorderedHashTable<Key, Key, SetKeyExtractor<Key>> UnorderedHashTable;
};

}  // namespace rart

#endif  // SRC_VM_HASH_SET_H_
