// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_MENUBAR_MENU_BAR_BACKDROP_H_
#define PROPERDESKTOP_SHELL_MENUBAR_MENU_BAR_BACKDROP_H_

#include "base/signal.h"
#include "shell/wallpaper/bar_backdrop.h"
#include "ui/geometry.h"

namespace properdesktop::shell {

class Wallpaper;

class MenuBarBackdrop
{
public:
    using Sample = BarBackdrop::Sample;
    using Style = BarBackdrop::Style;

    MenuBarBackdrop();
    ~MenuBarBackdrop();

    MenuBarBackdrop(const MenuBarBackdrop &) = delete;
    MenuBarBackdrop &operator=(const MenuBarBackdrop &) = delete;

    // Until then the bar follows the system's dark mode.
    bool isValid() const { return m_valid; }
    bool useLightForeground() const { return m_style.useLightForeground; }

    // Null stops.
    void follow(Wallpaper *wallpaper);
    void present(bool valid, const Style &style);
    void setColor(const ui::Color &color);
    void setUnavailable();

    base::Signal<> changed;

    static Sample sampleImage(const ui::Bitmap &screenImage, int barHeight)
    {
        return BarBackdrop::sampleImage(screenImage, barHeight);
    }
    static Style styleForSample(const Sample &sample) { return BarBackdrop::styleForSample(sample); }

private:
    Wallpaper *m_wallpaper = nullptr;
    base::Subscription m_judgementChanged;
    bool m_valid = false;
    Style m_style;
};

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_MENUBAR_MENU_BAR_BACKDROP_H_
