// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_CORE_PREFERENCES_H_
#define PROPERDESKTOP_CORE_PREFERENCES_H_

#include <string>
#include <string_view>

namespace properdesktop {

std::string preferencesDirectory();

// Each file has one writer, the process it is named after.
std::string preferencesFile(std::string_view process);

// Empty restores the user's.
void setConfigHomeForTesting(std::string configHome);

} // namespace properdesktop

#endif // PROPERDESKTOP_CORE_PREFERENCES_H_
