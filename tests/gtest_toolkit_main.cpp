// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <clocale>

#include <gtest/gtest.h>

#include "core/main_loop.h"
#include "core/preferences.h"
#include "temporary_directory.h"

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");
    testing::InitGoogleTest(&argc, argv);
    const properdesktop::test::TemporaryDirectory configHome;
    properdesktop::setConfigHomeForTesting(configHome.path());
    const properdesktop::core::MainLoop loop;
    const int result = RUN_ALL_TESTS();
    properdesktop::setConfigHomeForTesting({});
    return result;
}
