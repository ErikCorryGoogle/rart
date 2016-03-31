// Copyright (c) 2015, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_VM_HASH_MAP_H_
#define SRC_VM_HASH_MAP_H_

#include "src/hash_table.h"
#include "src/zone.h"

namespace rart {

template <typename Key, typename Mapped>
struct MapKeyExtractor {
  static Key& GetKey(Pair<Key, Mapped>& pair) {  // NOLINT
    return pair.first;
  }

  static const Key& GetKey(const Pair<Key, Mapped>& pair) { return pair.first; }
};

// HashMap:
// Interface is kept as close as possible to std::unordered_map, but:
// * Only functions that can be expected to be tiny are inlined.
// * The Key type must be memcpy-able and castable to void* and the same size.
// * The hash code is just a cast to int.
// * The Value type must be memcpy-able.
// * Not all methods are implemented.
// * Iterators are invalidated on all inserts, even if the key was already
//   present.
// * Google naming conventions are used (CamelCase classes and methods).
template <typename Key, typename Mapped>
class HashMap : public UnorderedHashTable<Key, Pair<Key, Mapped>,
                                          MapKeyExtractor<Key, Mapped>> {
 public:
  explicit HashMap(Zone* zone) : UnorderedHashTable(zone) {}

  // This is perhaps what you would think of as the value type, but that name
  // is used for the key-value pair.
  typedef Mapped MappedType;

  MappedType& AtPut(Zone* zone, const Key& key) {
    char* mapped = this->map_.LookUp(sizeof(Value), reinterpret_cast<const void*>(key), zone);
    return *reinterpret_cast<MappedType*>(mapped);
  }

  // Unlike the original zone-free API you can only use this for existing
  // entries.
  MappedType& operator[](const Key& key) {
    char* mapped = this->map_.At(sizeof(Value), reinterpret_cast<const void*>(key));
    ASSERT(mapped != NULL);
    return *reinterpret_cast<MappedType*>(mapped);
  }

 private:
  typedef Pair<Key, Mapped> Value;
  typedef UnorderedHashTable<Key, Pair<Key, Mapped>, MapKeyExtractor<Key, Mapped>> UnorderedHashTable;
};

}  // namespace rart

#endif  // SRC_VM_HASH_MAP_H_
