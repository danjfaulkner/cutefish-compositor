// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_COMMON_TEXT_MATCH_H_
#define PROPERDESKTOP_SHELL_COMMON_TEXT_MATCH_H_

#include <string>
#include <string_view>

namespace properdesktop::shell {

bool containsIgnoringCase(std::string_view text, std::string_view needle);

std::string toLower(std::string_view text);

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_COMMON_TEXT_MATCH_H_
