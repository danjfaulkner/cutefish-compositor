// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.chromium file.
//
// Upstream: chromium/src/components/exo/wayland/xdg_shell.cc
//           (WaylandXdgToplevelDecoration and the manager request table)
// Revision: 7acd40ddefff49c9d36144999aafdbb674dbd07a

#ifndef PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_DECORATION_H_
#define PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_DECORATION_H_

#include <wayland-server-core.h>

#include <cstdint>

namespace properdesktop::wayland {

constexpr uint32_t kZxdgDecorationManagerVersion = 1;

wl_global* createXdgDecorationGlobal(wl_display* display);

} // namespace properdesktop::wayland

#endif // PROPERDESKTOP_WAYLAND_PROTOCOLS_XDG_DECORATION_H_
