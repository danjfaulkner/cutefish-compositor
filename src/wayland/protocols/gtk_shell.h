// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

struct wl_display;
struct wl_global;

namespace properdesktop::wayland {

wl_global* createGtkShellGlobal(wl_display* display);

} // namespace properdesktop::wayland
