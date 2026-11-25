// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WAYLAND_MAIN_MENU_H
#define PROPERDESKTOP_WAYLAND_MAIN_MENU_H

#include <wayland-server-core.h>

namespace properdesktop::wayland {

struct WaylandSeat;

// A choice counts as input on the seat.
wl_global *createMainMenuGlobal(wl_display *display, WaylandSeat *seat);

} // namespace properdesktop::wayland

#endif
