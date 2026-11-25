// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.chromium file.
//
// Upstream: chromium/src/ash/wm/window_positioner.{h,cc}
//           (GetBoundsAndShowStateForNewWindow, GetReferenceWindow)
// Revision: 7acd40ddefff49c9d36144999aafdbb674dbd07a
//
// Adapted for Proper Desktop: a new window cascades from its application's latest one
// instead of taking its bounds, since the client chose its size before mapping.

#ifndef PROPERDESKTOP_WM_WINDOW_POSITIONER_H_
#define PROPERDESKTOP_WM_WINDOW_POSITIONER_H_

#include "base/geometry.h"

namespace properdesktop::wm {

class Window;

namespace window_positioner {

base::Rect boundsForNewWindow(const Window *window, base::Rect bounds);

} // namespace window_positioner

} // namespace properdesktop::wm

#endif // PROPERDESKTOP_WM_WINDOW_POSITIONER_H_
