// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WAYLAND_SURFACE_BLUR_H_
#define PROPERDESKTOP_WAYLAND_SURFACE_BLUR_H_

#include <wayland-server-core.h>

namespace properdesktop::ui {
class Path;
}

namespace properdesktop::wayland {

wl_global* createSurfaceBlurGlobal(wl_display* display);

// properdesktop_surface_blur_v1.path_command values and wl_fixed points; false when malformed or past 4096 values.
bool pathFromArray(const wl_array* array, ui::Path* result);

} // namespace properdesktop::wayland

#endif
