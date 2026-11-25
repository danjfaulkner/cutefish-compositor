// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_TOOLKIT_COMPOSITOR_COLORS_H_
#define PROPERDESKTOP_TOOLKIT_COMPOSITOR_COLORS_H_

#include <cstdint>

#include "ui/color_provider.h"

namespace properdesktop::toolkit {

enum class CompositorColor : uint16_t {
    SwitcherName = uint16_t(ui::ColorId::ApplicationFirst),

    DockDivider,

    LevelTrackBackground,
    LevelTrackOutline,
    LevelTrackFill,
    LevelTrackKnob,
    LevelTrackKnobShadow,
    LevelTrackSunkenBackground,
    LevelTrackSunkenOutline,
    LevelTrackSunkenKnobOutline,
    LevelTrackSunkenKnobShadow,

    ClipboardSurface,
    ClipboardHairline,

    NotificationSecondaryText,
    NotificationBannerSurface,
    NotificationCentreCard,
    NotificationCentreEmptyText,
};

void addCompositorColors(ui::ColorMixer &mixer, const ui::Theme &theme);

// Added before main by every program that names one of the colours.
[[maybe_unused]] inline const bool compositorColorsAdded = (ui::ColorProvider::addMixer(addCompositorColors), true);

} // namespace properdesktop::toolkit

#endif // PROPERDESKTOP_TOOLKIT_COMPOSITOR_COLORS_H_
