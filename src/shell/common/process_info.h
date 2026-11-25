// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_COMMON_PROCESS_INFO_H_
#define PROPERDESKTOP_SHELL_COMMON_PROCESS_INFO_H_

#include <cstdint>
#include <string>

namespace properdesktop::shell {

struct ProcessCommand {
    std::string program;
    std::string name;
};

// Empty when /proc cannot say; a script started through its shebang reports the
// script, not the interpreter.
ProcessCommand processCommand(int64_t pid);

// The parent process, or 0.
int64_t parentProcess(int64_t pid);

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_COMMON_PROCESS_INFO_H_
