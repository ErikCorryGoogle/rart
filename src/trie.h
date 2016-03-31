// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#ifndef SRC_TRIE_H_
#define SRC_TRIE_H_

#include "src/hash_map.h"
#include "src/zone.h"

namespace rart {

template<typename T>
class TrieNode : public ZoneAllocated {
 public:
  explicit TrieNode(Zone* zone) : map_(zone) {}

  inline T* LookupChild(int id) {
    auto it = map_.Find(id);
    if (it == map_.End()) return NULL;
    return it->second;
  }

  inline T* Child(Zone* zone, int id) {
    auto it = map_.Find(id);
    if (it != map_.End()) return it->second;
    return NewChild(zone, id);
  }

 private:
  HashMap<long, T*> map_;

  T* NewChild(Zone* zone, int id) {
    T* child = new(zone) T(zone);
    map_.AtPut(zone, id) = child;
    return child;
  }
};

}  // namespace rart

#endif  // SRC_TRIE_H_
