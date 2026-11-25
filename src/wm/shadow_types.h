// Copyright 2015 The Chromium Authors
// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.chromium file.
//
// Upstream: chromium/src/ui/wm/core/shadow_types.{h,cc}
// Revision: 7acd40ddefff49c9d36144999aafdbb674dbd07a

#ifndef PROPERDESKTOP_WM_SHADOW_TYPES_H_
#define PROPERDESKTOP_WM_SHADOW_TYPES_H_

namespace properdesktop::wm {

enum class ShadowElevation {
    None,
    Popup,
    Dock,
    MenuOrTooltip,
    Inactive,
    Active,
};

const char *toString(ShadowElevation elevation);

} // namespace properdesktop::wm

#endif // PROPERDESKTOP_WM_SHADOW_TYPES_H_
