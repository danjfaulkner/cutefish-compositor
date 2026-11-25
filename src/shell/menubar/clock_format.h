// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_MENUBAR_CLOCK_FORMAT_H_
#define PROPERDESKTOP_SHELL_MENUBAR_CLOCK_FORMAT_H_

#include <string>

namespace properdesktop::shell::clock_format {

std::string pattern(const std::string &localeName, bool twentyFourHour);

// Empty when the pattern has a field that format cannot show.
std::string toLocaleFormat(const std::string &icuPattern);

bool showsSeconds(const std::string &format);

} // namespace properdesktop::shell::clock_format

#endif // PROPERDESKTOP_SHELL_MENUBAR_CLOCK_FORMAT_H_
