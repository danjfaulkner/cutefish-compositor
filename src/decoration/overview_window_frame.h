// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_DECORATION_OVERVIEW_WINDOW_FRAME_H_
#define PROPERDESKTOP_DECORATION_OVERVIEW_WINDOW_FRAME_H_

#include "scene/overview_node.h"

namespace properdesktop::decoration {

class Theme;

scene::OverviewWindowFrame overviewWindowFrame(const Theme *theme);

} // namespace properdesktop::decoration

#endif // PROPERDESKTOP_DECORATION_OVERVIEW_WINDOW_FRAME_H_
