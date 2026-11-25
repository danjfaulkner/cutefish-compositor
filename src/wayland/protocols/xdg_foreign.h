// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_FOREIGN_H_
#define PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_FOREIGN_H_

#include <wayland-server-core.h>

#include <cstdint>

namespace properdesktop::wayland {

constexpr uint32_t kXdgExporterV2Version = 1;
constexpr uint32_t kXdgImporterV2Version = 1;

wl_global *createXdgExporterV2Global(wl_display *display);
wl_global *createXdgImporterV2Global(wl_display *display);

} // namespace properdesktop::wayland

#endif // PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_FOREIGN_H_
