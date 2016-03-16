// Copyright (c) 2014, the Rart project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "src/tokens.h"

namespace rart {

int Tokens::precedence_[] = {
#define T(n, s, p) p,
TOKEN_LIST(T)
#undef T
};

const char* Tokens::syntax_[] = {
#define T(n, s, p) s,
TOKEN_LIST(T)
#undef T
};

}  // namespace rart
