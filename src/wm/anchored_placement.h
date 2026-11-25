// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WM_ANCHORED_PLACEMENT_H_
#define PROPERDESKTOP_WM_ANCHORED_PLACEMENT_H_

#include "base/geometry.h"
#include "compositor/surface.h"

namespace properdesktop::wm {

// Places as an xdg_positioner with flipping and sliding on both axes places a popup, within `workArea`.
base::Rect placeAtAnchor(const base::Size &size, const compositor::PlacementAnchor &anchor,
                         const base::Rect &workArea);

} // namespace properdesktop::wm

#endif // PROPERDESKTOP_WM_ANCHORED_PLACEMENT_H_
