// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "src/test_case.h"

#include <stdio.h>
#include <string.h>

namespace rart {

TestCase* TestCase::first_ = NULL;
TestCase* TestCase::current_ = NULL;

TestCase::TestCase(RunEntry* run, const char* name)
    : next_(NULL),
      run_(run),
      name_(name) {
  if (first_ == NULL) {
    first_ = this;
  } else {
    current_->next_ = this;
  }
  current_ = this;
}

void TestCase::Run() {
  fprintf(stderr, "%s\n", name_);
  (*run_)();
}

void TestCase::RunAll() {
  TestCase* test_case = first_;
  while (test_case != NULL) {
    bool run = true;
    if (run) test_case->Run();
    test_case = test_case->next_;
  }
}

}  // namespace rart

int main(int argc, const char** argv) {
  rart::TestCase::RunAll();
  fprintf(stderr, "100%%\n");
  return 0;
}

