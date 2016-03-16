// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "src/assert.h"

#include <stdarg.h>

#include <cstdlib>

namespace rart {

void DynamicAssertionHelper::Fail(const char* format, ...) {
  // Print out the error.
  fprintf(stderr, "%s:%d: error: ", file_, line_);
  va_list arguments;
  va_start(arguments, format);
  char buffer[KB];
  vsnprintf(buffer, sizeof(buffer), format, arguments);
  va_end(arguments);
  fprintf(stderr, "%s\n", buffer);

  // In case of failed assertions, abort right away. Otherwise, wait
  // until the program is exiting before producing a non-zero exit
  // code through abort.
  if (kind_ == ASSERT) std::abort();
  static bool failed = false;
  if (!failed) std::atexit(std::abort);
  failed = true;
}

}  // namespace rart
