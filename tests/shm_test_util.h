// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_TESTS_SHM_TEST_UTIL_H_
#define PROPERDESKTOP_TESTS_SHM_TEST_UTIL_H_

#include <sys/mman.h>
#include <unistd.h>

#include <cstddef>
#include <memory>

#include "compositor/shm_pool.h"

namespace properdesktop::testsupport {

// A memfd of `size` bytes, owned by the caller.
inline int makeMemfd(size_t size)
{
    const int fd = memfd_create("properdesktop-test", MFD_CLOEXEC);
    if (fd < 0)
        return -1;
    if (ftruncate(fd, static_cast<off_t>(size)) != 0) {
        close(fd);
        return -1;
    }
    return fd;
}

inline std::shared_ptr<compositor::ShmPool> makePool(size_t size)
{
    const int fd = makeMemfd(size);
    return fd < 0 ? nullptr : compositor::ShmPool::create(fd, static_cast<int32_t>(size));
}

} // namespace properdesktop::testsupport

#endif // PROPERDESKTOP_TESTS_SHM_TEST_UTIL_H_
