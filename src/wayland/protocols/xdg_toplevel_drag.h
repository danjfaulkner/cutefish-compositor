// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_TOPLEVEL_DRAG_H_
#define PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_TOPLEVEL_DRAG_H_

#include <wayland-server-core.h>

#include <cstdint>

namespace properdesktop::wm {
class WindowManager;
}

namespace properdesktop::wayland {

constexpr uint32_t kXdgToplevelDragManagerVersion = 1;

wl_global *createXdgToplevelDragGlobal(wl_display *display, wm::WindowManager *windowManager);

} // namespace properdesktop::wayland

#endif // PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_TOPLEVEL_DRAG_H_
