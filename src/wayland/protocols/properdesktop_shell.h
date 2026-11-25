// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WAYLAND_PROTOCOLS_PROPERDESKTOP_SHELL_H_
#define PROPERDESKTOP_WAYLAND_PROTOCOLS_PROPERDESKTOP_SHELL_H_

#include <wayland-server-core.h>

#include <cstdint>

namespace properdesktop::wm {
class WindowManager;
}

namespace properdesktop::wayland {

class PrivilegedClients;

constexpr uint32_t kProperDesktopShellVersion = 1;

class WaylandProperDesktopShell
{
public:
    WaylandProperDesktopShell(wm::WindowManager* windowManager, PrivilegedClients* privileged);
    WaylandProperDesktopShell(const WaylandProperDesktopShell&) = delete;
    WaylandProperDesktopShell& operator=(const WaylandProperDesktopShell&) = delete;

    wm::WindowManager* windowManager() const { return m_windowManager; }
    PrivilegedClients* privileged() const { return m_privileged; }

private:
    wm::WindowManager* m_windowManager = nullptr;
    PrivilegedClients* m_privileged = nullptr;
};

// `shell` must outlive the global; registering it with the privileged registry hides it from ordinary clients.
wl_global* createProperDesktopShellGlobal(wl_display* display, WaylandProperDesktopShell* shell);

} // namespace properdesktop::wayland

#endif // PROPERDESKTOP_WAYLAND_PROTOCOLS_PROPERDESKTOP_SHELL_H_
