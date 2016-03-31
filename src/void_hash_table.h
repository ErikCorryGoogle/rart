// Copyright (c) 2015, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.
//
#ifndef SRC_VM_VOID_HASH_TABLE_H_
#define SRC_VM_VOID_HASH_TABLE_H_

#include "src/globals.h"
#include "src/assert.h"
#include "src/zone.h"

namespace rart {

class VoidHashTable {
 public:
  VoidHashTable(size_t pair_size, Zone* zone);
  ~VoidHashTable();

  char* Find(size_t pair_size, const void* key);

  const char* Find(size_t pair_size, const void* key) const {
    return const_cast<VoidHashTable*>(this)->Find(pair_size, key);
  }

  char* At(size_t pair_size, const void* key);

  char* Insert(size_t pair_size, Zone* zone, const void* key, const char* pair, bool* inserted);

  char* Erase(size_t pair_size, const char* entry);

  void Swap(size_t pair_size, VoidHashTable& other);

  void Clear(size_t pair_size, Zone* zone);

  // Used to implement the [] operator.  Returns address of value part of
  // key-value pair.
  char* LookUp(size_t pair_size, const void* key, Zone* zone);

  size_t size() const { return size_; }

  const char* backing() const { return backing_; }

  char* backing() { return backing_; }

  const char* backing_end() const { return backing_end_; }

  char* backing_end() { return backing_end_; }

#ifdef DEBUG
  size_t mutations() const { return mutations_; }
#endif

  template <typename Pointer>
  class Iterator {
   public:
    inline Iterator(const VoidHashTable* table, Pointer position,
                    size_t pair_size)
        : position_(position)
#ifdef DEBUG
          ,
          table_(table),
          mutations_(table == NULL ? 0 : table->mutations())
#endif
          {
    }

    template <typename OtherPointer>
    inline Iterator(const Iterator<OtherPointer>& other)
        : position_(other.position_)
#ifdef DEBUG
          ,
          table_(other.table_),
          mutations_(other.mutations_)
#endif
          {
    }

    inline void AdvanceToUsedSlot(size_t pair_size) {
      if (position_ == NULL) return;
      while (IsUnused(position_)) {
        Increment(pair_size);
      }
    }

    inline void Increment(size_t pair_size) {
#ifdef DEBUG
      ASSERT(mutations_ == table_->mutations());
#endif
      position_ += EntrySize(pair_size);
    }

    Pointer operator*() const {
#ifdef DEBUG
      ASSERT(mutations_ == table_->mutations());
#endif
      // Skip the hash code.
      return position_ + sizeof(hash_t);
    }

    Pointer position() { return position_; }

    template <typename OtherPointer>
    bool operator==(const Iterator<OtherPointer>& other) const {
      return other.position_ == position_;
    }

    template <typename OtherPointer>
    bool operator!=(const Iterator<OtherPointer>& other) const {
      return !(*this == other);
    }

    Pointer position_;
#ifdef DEBUG
    const VoidHashTable* table_;
    size_t mutations_;
#endif
  };

  typedef intptr_t hash_t;

 private:
  void Rehash(size_t pair_size, Zone* zone, size_t new_capacity);

  void AllocateBacking(size_t pair_size, size_t capacity, Zone* zone);

  char* FindStopBucket(size_t pair_size, char* entry);

  size_t capacity() { return mask_ + 1; }

  char* RawFind(size_t pair_size, const void* key, bool* inserted_return, Zone* zone);

  static inline const void** KeyFromEntry(char* entry) {
    return reinterpret_cast<const void**>(entry + sizeof(hash_t));
  }

  static inline void* const* KeyFromEntry(const char* entry) {
    return reinterpret_cast<void* const*>(entry + sizeof(hash_t));
  }

  static inline char* PairFromEntry(char* entry) {
    return entry + sizeof(hash_t);
  }

  static inline const char* PairFromEntry(const char* entry) {
    return entry + sizeof(hash_t);
  }

  static inline char* ValueFromEntry(char* entry) {
    return entry + sizeof(hash_t) + sizeof(void*);
  }

  static size_t EntrySize(size_t pair_size) { return pair_size + sizeof(hash_t); }
  static size_t SizeOfValue(size_t pair_size) { return pair_size - sizeof(void*); }

  // Measured in entries.
  static const size_t kInitialCapacity = 4;
  // Raw hash code of unused slot.
  static const hash_t kUnusedSlot = -1;
  // Raw hash code of position after the end.
  static const hash_t kPastTheEnd = 0x446e45;  // EnD.

  static void* GetKey(const char* bucket);

  void SwapEntries(size_t pair_size, char* p1, char* p2);

  static hash_t HashCode(const void* key);

  static inline hash_t StoredHashCode(const char* entry) {
    return *reinterpret_cast<const hash_t*>(entry);
  }

  static inline void SetHashCode(char* entry, hash_t code) {
    ASSERT(code >= 0);
    *reinterpret_cast<hash_t*>(entry) = code;
  }

  static inline void SetUnused(char* entry) {
    *reinterpret_cast<hash_t*>(entry) = kUnusedSlot;
  }

  static inline bool IsUnused(const char* entry) {
    hash_t raw_code = StoredHashCode(entry);
    return raw_code < 0;
  }

  size_t mask_ = 0;           // Capacity (in entries) - 1.
#ifdef DEBUG
  size_t mutations_ = 0;
#endif
  size_t size_ = 0;           // Entries in use.
  char* backing_ = NULL;      // The backing store of entries.
  char* backing_end_ = NULL;  // The end of the backing store of entries.
};

}  // namespace rart.

#endif  // SRC_VM_VOID_HASH_TABLE_H_
