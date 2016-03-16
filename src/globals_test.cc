// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#define TESTING

#include "src/assert.h"
#include "src/globals.h"
#include "src/test_case.h"

namespace rart {

TEST_CASE(TypeSizes) {
  EXPECT_EQ(1U, sizeof(u8));
  EXPECT_EQ(1U, sizeof(i8));

  EXPECT_EQ(2U, sizeof(u16));
  EXPECT_EQ(2U, sizeof(i16));

  EXPECT_EQ(4U, sizeof(u32));
  EXPECT_EQ(4U, sizeof(i32));

  EXPECT_EQ(8U, sizeof(u64));
  EXPECT_EQ(8U, sizeof(i64));
}

TEST_CASE(ArraySize) {
  static int i1[] = { 1 };
  EXPECT_EQ(1U, ARRAY_SIZE(i1));
  static int i2[] = { 1, 2 };
  EXPECT_EQ(2U, ARRAY_SIZE(i2));
  static int i3[3] = { 0, };
  EXPECT_EQ(3U, ARRAY_SIZE(i3));

  static char c1[] = { 1 };
  EXPECT_EQ(1U, ARRAY_SIZE(c1));
  static char c2[] = { 1, 2 };
  EXPECT_EQ(2U, ARRAY_SIZE(c2));
  static char c3[3] = { 0, };
  EXPECT_EQ(3U, ARRAY_SIZE(c3));
}

}  // namespace rart
