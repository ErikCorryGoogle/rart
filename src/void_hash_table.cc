// Copyright (c) 2015, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include <string.h>

#include "src/void_hash_table.h"

namespace rart {

VoidHashTable::hash_t VoidHashTable::HashCode(const void* key) {
  return reinterpret_cast<intptr_t>(key) & INTPTR_MAX;
}

VoidHashTable::VoidHashTable(size_t pair_size, Zone* zone) {}

VoidHashTable::~VoidHashTable() {}

void VoidHashTable::AllocateBacking(size_t pair_size, size_t capacity, Zone* zone) {
  size_t length = EntrySize(pair_size) * capacity + sizeof(hash_t);
  backing_ = reinterpret_cast<char*>(zone->Allocate(length));
  backing_end_ = backing_ + length - sizeof(hash_t);
  for (char* hashes = backing_; hashes < backing_end_; hashes += EntrySize(pair_size)) {
    *reinterpret_cast<hash_t*>(hashes) = kUnusedSlot;
  }
  // An iterator is incremented by finding the next entry with a valid hash
  // code.  This is a dummy valid hash code that ensures the iterator can
  // advance to the end.
  SetHashCode(backing_end_, kPastTheEnd);
}

void* VoidHashTable::GetKey(const char* bucket) {
  return *KeyFromEntry(bucket);
}

void VoidHashTable::SwapEntries(size_t pair_size, char* p1, char* p2) {
  for (size_t i = 0; i < EntrySize(pair_size); i += sizeof(hash_t)) {
    hash_t temp = *reinterpret_cast<hash_t*>(p1 + i);
    *reinterpret_cast<hash_t*>(p1 + i) = *reinterpret_cast<hash_t*>(p2 + i);
    *reinterpret_cast<hash_t*>(p2 + i) = temp;
  }
}

// If inserted is not null, create the entry if it does not exist.  Write
// 'true' to 'inserted' if we created the entry.  The zone can be null
// is 'inserted' is null.
char* VoidHashTable::RawFind(size_t pair_size, const void* key, bool* inserted, Zone* zone) {
  // Using >> 3 here would mean max 88% occupancy.  With >> 2 we have max 75% occupancy, which
  // gives a 3% performance improvement.
  if (inserted != NULL) {
    if (size_ + (size_ >> 2) >= mask_) Rehash(pair_size, zone, capacity() * 2);
#ifdef DEBUG
    mutations_++;
#endif
  } else if (backing_ == NULL) {
    return NULL;
  }
  // Answer is the slot we will return, but it's also the location of any data
  // we are carrying forward.
  char* answer = NULL;
  intptr_t hash_code = HashCode(key);
  intptr_t ideal_position = hash_code & mask_;
  intptr_t current_position = ideal_position;
  char* bucket = backing_ + EntrySize(pair_size) * current_position;
  while (true) {
    if (IsUnused(bucket)) {
      if (inserted == NULL) return NULL;
      *inserted = true;
      size_++;
      if (answer == NULL) {
        SetHashCode(bucket, hash_code);
        return bucket;
      }
      memcpy(bucket, answer, EntrySize(pair_size));
      SetHashCode(answer, hash_code);
      return answer;
    } else if (GetKey(bucket) == key) {
      ASSERT(hash_code == StoredHashCode(bucket));
      // We can't have found an entry that needed bumping if the key was
      // already in the map.
      ASSERT(answer == NULL);
      return bucket;
    }
    intptr_t entry_ideal_position = StoredHashCode(bucket) & mask_;
    intptr_t entry_distance = (current_position - entry_ideal_position) & mask_;
    if (entry_distance < current_position - ideal_position) {
      if (inserted == NULL) return NULL;
      if (answer == NULL) {
        answer = bucket;
      } else {
        // Swap them around, so the current bucket goes to 'answer' (the data
        // we are carrying forward) and the data we were carrying goes here.
        SwapEntries(pair_size, answer, bucket);
      }
      ideal_position = entry_ideal_position;
    }
    current_position++;
    bucket += EntrySize(pair_size);
    if (bucket == backing_end_) bucket = backing_;
  }
}

char* VoidHashTable::Find(size_t pair_size, const void* key) {
  char* existing_entry = RawFind(pair_size, key, NULL, NULL);
  if (existing_entry == NULL) return backing_end_;
  return existing_entry;
}

char* VoidHashTable::At(size_t pair_size, const void* key) {
  char* entry = RawFind(pair_size, key, NULL, NULL);
  if (entry == NULL) return entry;
  return ValueFromEntry(entry);
}

char* VoidHashTable::LookUp(size_t pair_size, const void* key, Zone* zone) {
  bool inserted = false;
  char* entry = RawFind(pair_size, key, &inserted, zone);
  if (inserted) {
    *KeyFromEntry(entry) = key;
    memset(ValueFromEntry(entry), 0, SizeOfValue(pair_size));
  }
  return ValueFromEntry(entry);
}

void VoidHashTable::Rehash(size_t pair_size, Zone* zone, size_t capacity) {
  char* old_backing = backing_;
  char* old_backing_end = backing_end_;
  mask_ = capacity - 1;
  size_ = 0;
  AllocateBacking(pair_size, capacity, zone);

  for (char* p = old_backing; p < old_backing_end; p += EntrySize(pair_size)) {
    if (!IsUnused(p)) {
      const void* key = GetKey(p);
      bool was_inserted = false;
      char* new_entry = RawFind(pair_size, key, &was_inserted, zone);
      ASSERT(was_inserted);
      memcpy(new_entry, p, EntrySize(pair_size));
    }
  }
}

char* VoidHashTable::Insert(size_t pair_size, Zone* zone, const void* key, const char* pair, bool* inserted) {
  char* entry = RawFind(pair_size, key, inserted, zone);
  memcpy(PairFromEntry(entry), pair, pair_size);
  return entry;
}

void VoidHashTable::Swap(size_t pair_size, VoidHashTable& other) {
  size_t t;
  t = mask_;
  mask_ = other.mask_;
  other.mask_ = t;
#ifdef DEBUG
  t = mutations_;
  mutations_ = other.mutations_;
  other.mutations_ = t;
#endif
  t = size_;
  size_ = other.size_;
  other.size_ = t;
  char* t2;
  t2 = backing_;
  backing_ = other.backing_;
  other.backing_ = t2;
  t2 = backing_end_;
  backing_end_ = other.backing_end_;
  other.backing_end_ = t2;
}

char* VoidHashTable::FindStopBucket(size_t pair_size, char* entry) {
  while (true) {
    entry += EntrySize(pair_size);
    if (entry == backing_end_) entry = backing_;
    if (IsUnused(entry)) return entry;
    size_t ideal_position = StoredHashCode(entry) & mask_;
    if (backing_ + ideal_position * EntrySize(pair_size) == entry) return entry;
  }
}

char* VoidHashTable::Erase(size_t pair_size, const char* entry) {
#ifdef DEBUG
  mutations_++;
#endif
  // It's OK to delete using a const pointer from a const_iterator because that
  // only says that the individual entries are const, not that the collection
  // is const (and you can delete const things, because that doesn't mutate
  // them it just causes them to stop existing).
  char* position = const_cast<char*>(entry);
  // We need to move down the later elements to fill the gap left by the
  // deleted element.
  char* dest = position;
  char* stop_bucket = FindStopBucket(pair_size, position);
  if (dest > stop_bucket) {
    // Wraparound case.
    memmove(dest, dest + EntrySize(pair_size), backing_end_ - (dest + EntrySize(pair_size)));
    if (stop_bucket > backing_) {
      memcpy(backing_end_ - EntrySize(pair_size), backing_, EntrySize(pair_size));
      memmove(backing_, backing_ + EntrySize(pair_size),
              stop_bucket - (backing_ + EntrySize(pair_size)));
    }
  } else if (stop_bucket != dest) {
    size_t len = (stop_bucket - dest) - EntrySize(pair_size);
    ASSERT(len < INTPTR_MAX);  // No unsigned wrap around.
    memmove(dest, dest + EntrySize(pair_size), len);
  }
  // Mark the one before the stop position as unused.
  if (stop_bucket == backing_) {
    SetUnused(backing_end_ - EntrySize(pair_size));
  } else {
    SetUnused(stop_bucket - EntrySize(pair_size));
  }

  // Don't rehash here, because the contract is that the iterator can still be
  // used.
  size_--;
  return position;
}

void VoidHashTable::Clear(size_t pair_size, Zone* zone) {
  if (size_ == 0) return;
#ifdef DEBUG
  mutations_++;
#endif
  mask_ = size_ = 0;
  backing_ = backing_end_ = NULL;
}

}  // namespace rart.
