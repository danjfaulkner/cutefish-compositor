// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WAYLAND_STATUS_ITEM_H
#define PROPERDESKTOP_WAYLAND_STATUS_ITEM_H

#include <wayland-server-core.h>

namespace properdesktop::wm {
class StatusItems;
}

namespace properdesktop::wayland {

wl_global *createStatusItemGlobal(wl_display *display, wm::StatusItems *items);

} // namespace properdesktop::wayland

#endif
