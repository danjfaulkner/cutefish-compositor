// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_COMMON_FILE_THREAD_H_
#define PROPERDESKTOP_SHELL_COMMON_FILE_THREAD_H_

#include <functional>
#include <string>

#include "base/settings.h"
#include "base/thread_pool.h"

namespace properdesktop::shell {

// Runs work in order, so a write never overtakes an earlier one to the same
// file.
base::ThreadPool &fileThread();

template<typename Work, typename Done>
base::Job runOnFileThread(Work work, Done done)
{
    return fileThread().run(std::move(work), std::move(done));
}

void waitForFileThread();

// Applies `change` to the file as it is by then, so components sharing
// shell.conf keep each other's keys.
void writeSettings(const std::string &path, std::function<void(base::Settings &settings)> change);

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_COMMON_FILE_THREAD_H_
