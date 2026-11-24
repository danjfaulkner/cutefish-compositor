// Copyright 2026 danjfaulkner
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CUTEFISH_WM_DESKS_FULLSCREEN_DESKS_H_
#define CUTEFISH_WM_DESKS_FULLSCREEN_DESKS_H_

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <string>

#include "base/signal.h"
#include "wm/desks/desks_controller.h"
#include "wm/types.h"
#include "wm/window_observer.h"

namespace cutefish::wm {

class Desk;
class Window;
class WindowManager;

// macOS-style fullscreen: the window goes fullscreen on a desk of its own, named after its
// application and placed right after the desk it came from. Leaving fullscreen, closing the
// window or moving it away puts it back on that desk and removes the fullscreen desk.
// Fullscreen that a client asks for itself (F11, video) is left to WindowState, in place.
class FullscreenDesks : public DesksObserver, public WindowObserver
{
public:
    // How long a client gets to draw at its fullscreen size before its desk is switched to.
    static constexpr std::chrono::milliseconds kSettleTimeout {500};
    // Leaving fullscreen this soon after entering it, on its own, is a client refusing it.
    static constexpr std::chrono::milliseconds kRefusalWindow {3000};

    explicit FullscreenDesks(WindowManager *manager);
    FullscreenDesks(const FullscreenDesks &) = delete;
    FullscreenDesks &operator=(const FullscreenDesks &) = delete;
    ~FullscreenDesks() override;

    // A normal, resizable application window that is not fullscreen in place.
    bool canEnter(const Window *window) const;
    // Why canEnter() is false, for logs and the window debug service; empty when it is true.
    std::string refusal(const Window *window) const;
    // Fullscreen on a desk of its own.
    bool owns(const Window *window) const;
    // Owned, and filling the output as a maximized window rather than in the fullscreen state:
    // for clients that do not take fullscreen they did not ask for (Chromium browsers leave it
    // again by themselves), which keep their own toolbars that way, as on macOS.
    bool fills(const Window *window) const;
    bool isFullscreenDesk(const Desk *desk) const;
    // The desk a fullscreen desk's window came from; null when `desk` is not one.
    Desk *originOf(const Desk *desk) const;

    // Enters, or leaves when owned. A window fullscreen in place just leaves fullscreen.
    // False when nothing was done.
    bool toggle(Window *window);
    bool enter(Window *window);
    void exit(Window *window);

    // The desk's name for a window; the shell installs one that reads the desktop entry.
    void setNameResolver(std::function<std::string(const Window *)> resolver)
    {
        m_nameResolver = std::move(resolver);
    }

    // After entering or leaving.
    base::Signal<> changed;

    void onWindowRemoving(Window *window);

    // DesksObserver
    void onDeskRemoved(const Desk *desk) override;
    void onWindowDeskChanged(Window *window) override;

    // WindowObserver
    void onWindowStateTypeChanged(Window *window, WindowStateType oldType) override;
    void onWindowContentChanged(Window *window) override;

private:
    struct Entry {
        uint32_t desk = 0;
        uint32_t origin = 0;
        bool fill = false;
        WindowStateType previous = WindowStateType::Normal;
        std::chrono::steady_clock::time_point entered;
        // On its own desk yet: it goes fullscreen where it is first.
        bool moved = false;
    };


    bool prefersFill(const Window *window) const;
    void applyFill(Window *window);
    // Once fullscreen where it is, the window goes to its own desk, switched to at once.
    void moveToOwnDesk(Window *window);
    // Shows `window` on every desk while `to` becomes active at once, then pins it to `to`:
    // it is never hidden and keeps activation throughout.
    void handOver(Window *window, Desk *to);
    // Back to the state the window had before, from fullscreen or fill.
    void restoreState(Window *window, const Entry &entry);

    DesksController *desks() const;
    std::string nameFor(const Window *window) const;
    Desk *originFor(const Entry &entry) const;
    void forget(Window *window);
    // Removes the fullscreen desk once nothing is left on it.
    void removeDeskLater(uint32_t deskId, uint64_t activateWindow);

    WindowManager *m_manager = nullptr;
    std::map<Window *, Entry> m_entries;
    // Fullscreen desk id to the window that gets activation once the desk is gone.
    std::map<uint32_t, uint64_t> m_activateOnRemoval;
    std::function<std::string(const Window *)> m_nameResolver;
    // Applications seen refusing fullscreen in this session.
    std::set<std::string> m_fillApplications;
    bool m_changing = false;
};

} // namespace cutefish::wm

#endif // CUTEFISH_WM_DESKS_FULLSCREEN_DESKS_H_
