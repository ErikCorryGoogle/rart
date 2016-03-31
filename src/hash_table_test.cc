// Copyright (c) 2015, the Rart project authors. Please see the AUTHORS file
// for details. all rights reserved. use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include <string.h>

#define TESTING

#include "src/assert.h"
#include "src/hash_map.h"
#include "src/hash_set.h"
#include "src/pair.h"
#include "src/test_case.h"
#include "src/zone.h"

namespace rart {

TEST_CASE(StringMap) {
  Zone zone;
  typedef HashMap<const char*, const char*> Map;
  Map map1(&zone);
  EXPECT(map1.Empty());
  EXPECT_EQ(map1.size(), 0u);
  EXPECT(map1.Begin() == map1.End());
  map1.Insert(&zone, {"foo", "bar"});
  EXPECT_EQ(map1.size(), 1u);

  {
    Map::Iterator it = map1.Begin();
    EXPECT_EQ(it->first, "foo");
    EXPECT_EQ(it->second, "bar");
    ++it;
    EXPECT(it == map1.End());
  }

  map1.Insert(&zone, {"baz", "fizz"});
  EXPECT_EQ(map1.size(), 2u);

  {
    Map::Iterator it = map1.Begin();
    if (!strcmp(it->first, "foo")) {
      EXPECT_EQ(it->second, "bar");
      ++it;
    }
    EXPECT_EQ(it->first, "baz");
    EXPECT_EQ(it->second, "fizz");
    ++it;
    if (it != map1.End()) {
      EXPECT_EQ(it->first, "foo");
      EXPECT_EQ(it->second, "bar");
      ++it;
    }
  }

  {
    // Not found.
    Map::ConstIterator it = map1.Find("buzz");
    EXPECT(it == map1.End());
  }

  {
    // This works because the C++ compiler interns const strings, which is not
    // actually guaranteed.
    Map::ConstIterator it = map1.Find("foo");
    EXPECT(it != map1.End());

    EXPECT_EQ(it->second, "bar");
  }

  {
    Map::ConstIterator it = map1.Find("baz");
    EXPECT(it != map1.End());

    EXPECT_EQ(it->second, "fizz");
  }

  EXPECT_EQ(map1.size(), 2u);
  map1.Clear(&zone);
  EXPECT_EQ(map1.size(), 0u);
  EXPECT(map1.Begin() == map1.End());
  map1.Insert(&zone, {"baz", "fizz"});
  EXPECT_EQ(map1.size(), 1u);
  EXPECT_EQ(map1.Find("baz")->second, "fizz");
  EXPECT(map1.Find("foo") == map1.End());
}

TEST_CASE(StringSet) {
  Zone zone;
  typedef HashSet<const char*> Set;
  Set set1(&zone);
  EXPECT(set1.Empty());
  EXPECT_EQ(set1.size(), 0u);
  EXPECT(set1.Begin() == set1.End());
  set1.Insert(&zone, "foo");
  EXPECT_EQ(set1.size(), 1u);

  {
    Set::Iterator it = set1.Begin();
    EXPECT_EQ(*it, "foo");
    ++it;
    EXPECT(it == set1.End());
  }

  set1.Insert(&zone, "baz");
  EXPECT_EQ(set1.size(), 2u);

  {
    Set::Iterator it = set1.Begin();
    if (!strcmp(*it, "foo")) {
      ++it;
    }
    EXPECT_EQ(*it, "baz");
    ++it;
    if (it != set1.End()) {
      EXPECT_EQ(*it, "foo");
      ++it;
    }
  }

  {
    // Not found.
    Set::ConstIterator it = set1.Find("buzz");
    EXPECT(it == set1.End());
  }

  {
    // This works because the C++ compiler interns const strings, which is not
    // actually guaranteed.
    Set::ConstIterator it = set1.Find("foo");
    EXPECT(it != set1.End());
  }

  {
    Set::ConstIterator it = set1.Find("baz");
    EXPECT(it != set1.End());
  }

  EXPECT_EQ(set1.size(), 2u);
  set1.Clear(&zone);
  EXPECT_EQ(set1.size(), 0u);
  EXPECT(set1.Begin() == set1.End());
  set1.Insert(&zone, "baz");
  EXPECT_EQ(set1.size(), 1u);
  EXPECT_EQ(*set1.Find("baz"), "baz");
  EXPECT(set1.Find("foo") == set1.End());
}

typedef HashMap<intptr_t, int> IntInt;

void StillThere(IntInt* map1) {
  EXPECT_EQ(map1->Find(0)->first, 0);
  EXPECT_EQ(map1->Find(0)->second, 0);
  EXPECT_EQ(map1->Find(5)->first, 5);
  EXPECT_EQ(map1->Find(5)->second, 500);
  EXPECT_EQ(map1->Find(-5)->first, -5);
  EXPECT_EQ(map1->Find(-5)->second, -500);
}

TEST_CASE(IntMap) {
  Zone zone;
  IntInt map1(&zone);

  for (int i = -10; i < 10; i++) {
    if (i & 1) {
      map1.Insert(&zone, {i, i * 100});
    } else {
      map1.AtPut(&zone, i) = i * 100;
    }
  }

  StillThere(&map1);

  for (int i = -10; i < 10; i++) {
    if (i % 5 != 0) {
      unsigned s = map1.size();
      map1.Erase(map1.Find(i));
      StillThere(&map1);
      EXPECT_EQ(s - 1, map1.size());
    }
  }
}

void StillThereStrange(IntInt* map1) {
  EXPECT_EQ(0, map1->Find(0)->first);
  EXPECT_EQ(0, map1->Find(0)->second);
  EXPECT_EQ(5, map1->Find(5)->first);
  EXPECT_EQ(20, map1->Find(5)->second);
  EXPECT_EQ(10, map1->Find(10)->first);
  EXPECT_EQ(10, map1->Find(10)->second);
  EXPECT_EQ(15, map1->Find(15)->first);
  EXPECT_EQ(30, map1->Find(15)->second);
  EXPECT_EQ(20, map1->Find(20)->first);
  EXPECT_EQ(5, map1->Find(20)->second);
  EXPECT_EQ(25, map1->Find(25)->first);
  EXPECT_EQ(19, map1->Find(25)->second);
  EXPECT_EQ(30, map1->Find(30)->first);
  EXPECT_EQ(15, map1->Find(30)->second);
}

TEST_CASE(IntMapStrangeOrder) {
  Zone zone;
  IntInt map1(&zone);

  for (int i = 0; i < 32; i++) {
    int j = ((i & 1) << 4) | ((i & 2) << 2) | ((i & 4) << 0) | ((i & 8) >> 2) |
            ((i & 16) >> 4);
    map1.AtPut(&zone, j) = i;
  }

  StillThereStrange(&map1);

  for (int i = 0; i < 32; i++) {
    if (i % 5 != 0) {
      unsigned s = map1.size();
      map1.Erase(map1.Find(i));
      StillThereStrange(&map1);
      EXPECT_EQ(s - 1, map1.size());
    }
  }
}

typedef HashSet<intptr_t> IntSet;

void StillThere(IntSet* set1) {
  EXPECT_EQ(*(set1->Find(0)), 0);
  EXPECT_EQ(*(set1->Find(0)), 0);
  EXPECT_EQ(*(set1->Find(5)), 5);
  EXPECT_EQ(*(set1->Find(-5)), -5);
}

TEST_CASE(IntSet) {
  Zone zone;
  IntSet set1(&zone);

  for (int i = -10; i < 10; i++) {
    set1.Insert(&zone, i);
  }

  StillThere(&set1);

  for (int i = -10; i < 10; i++) {
    if (i % 5 != 0) {
      unsigned s = set1.size();
      set1.Erase(set1.Find(i));
      StillThere(&set1);
      EXPECT_EQ(s - 1, set1.size());
    }
  }
}

TEST_CASE(IntMapSpeedTest) {
  Zone zone;
  const int SIZE = 100;
  const int REPEAT = 1000;
  for (int i = 0; i < REPEAT; i++) {
    IntInt map1(&zone);
    for (int j = 0; j < SIZE; j++) {
      map1.AtPut(&zone, j) = j;
    }
    size_t total = 0;
    for (int j = 0; j < 10; j++) {
      for (IntInt::Iterator it = map1.Begin(); it != map1.End(); ++it) {
        total += it->second;
      }
      total += map1[42] + map1[7] + map1[53] + map1[87];
    }
    EXPECT_EQ(51390u, total);
  }
}

}  // namespace rart.
