// Copyright 2012 The Chromium Authors
// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.chromium file.
//
// Upstream: chromium/src/ash/shelf/shelf_layout_manager.{h,cc}
// Revision: 7acd40ddefff49c9d36144999aafdbb674dbd07a

#ifndef PROPERDESKTOP_WM_DOCK_LAYOUT_MANAGER_H_
#define PROPERDESKTOP_WM_DOCK_LAYOUT_MANAGER_H_

#include <chrono>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "base/geometry.h"
#include "base/signal.h"
#include "base/timer.h"
#include "core/main_loop.h"
#include "wm/types.h"
#include "wm/window_observer.h"

namespace properdesktop::wm {

class Window;
class WindowManager;

enum class DockAutoHideBehavior {
    Never,
    Always,
    WhenObstructed,
};

enum class DockVisibilityState {
    Visible,
    AutoHide,
};

enum class DockAutoHideState {
    Shown,
    Hidden,
};

enum class WorkspaceState {
    Default,
    Maximized,
    Fullscreen,
};

// Must be destroyed before the WindowManager it observes.
class DockLayoutManager : public WindowObserver
{
public:
    static constexpr int kRevealStripPx = 5;
    static constexpr int kAutoHideDelayMs = 200;
    // Over a fullscreen window the top bar comes in only when the pointer at the very top of the
    // output keeps pushing up past it this far, like macOS's resistance: resting on the edge or
    // passing along it does not bring it. Pushes further apart than kRevealPushResetMs start over.
    static constexpr double kFullscreenRevealPushPx = 150;
    static constexpr int kRevealPushResetMs = 700;
    static constexpr int kFullscreenRevealDelayMs = 0;
    static constexpr int kFullscreenRevealStripPx = 1;
    static constexpr int kSlideDurationMs = 200;

    explicit DockLayoutManager(WindowManager *manager);
    ~DockLayoutManager() override;

    DockAutoHideBehavior behavior() const { return m_behavior; }
    void setBehavior(DockAutoHideBehavior behavior);

    void setSessionStarting(bool starting);

    void setSessionEnding(bool ending);

    // Anything not treated as a dock is Visible and Shown.
    DockVisibilityState visibilityState(const Window *dock) const;
    DockAutoHideState autoHideState(const Window *dock) const;

    WorkspaceState workspaceStateFor(const base::Rect &output) const;

    void update();

    // `frameTimeMs` is on the output frame clock. Called before each frame is drawn.
    void advance(double frameTimeMs);
    bool isSliding(const Window *dock) const;

    void onWindowBoundsChanged(Window *window, const base::Rect &oldBounds) override;
    void onWindowStateTypeChanged(Window *window, WindowStateType oldType) override;
    void onWindowVisibilityChanged(Window *window, bool visible) override;
    void onWindowTransientParentChanged(Window *window, Window *oldParent) override;

private:
    void onWindowAdded(properdesktop::wm::Window *window);
    void onWindowRemoved(properdesktop::wm::Window *window);
    void onPointerMoved(const base::PointF &location);
    void onPointerPushedPastEdge(const base::PointF &overshoot);

private:
    struct Dock {
        DockVisibilityState visibility = DockVisibilityState::Visible;
        DockAutoHideState autoHide = DockAutoHideState::Shown;
        ScreenEdge edge = ScreenEdge::Bottom;
        base::Timer autoHideTimer {core::mainLoop()};
        bool pointerOver = false;
        bool sliding = false;
        double slideStarted = -1;
        base::Point slideFrom;
        base::Point slideTo;
        base::Point offset;
        bool parked = false;
        uint64_t revealedOver = 0;
        int revealSent = 0;
        uint64_t pushed = 0;
    };

    Dock *dockFor(Window *window);
    const Dock *dockFor(const Window *window) const;
    void enter(Window *dock, Dock &entry);
    void updateDock(Window *dock, Dock &entry);
    void updateAutoHideStateNow(Window *dock);
    DockVisibilityState computeVisibility(const Window *dock, const base::Rect &output) const;
    DockAutoHideState computeAutoHideState(const Window *dock, const Dock &entry,
                                           const base::Rect &output, bool revealing) const;
    bool isAwayForSession() const { return m_sessionStarting || m_sessionEnding; }
    bool hasVisibleWindow(const base::Rect &output) const;
    const Window *topmostWindowFor(const base::Rect &output) const;
    Window *fullscreenWindowFor(const base::Rect &output) const;
    bool ownsDesk(const Window *window) const;
    bool isTopBar(const Window *dock, const Dock &entry, const base::Rect &output) const;
    // The strip the pointer reveals the dock from: thinner for the top bar over fullscreen.
    base::Rect revealStripFor(const Window *dock, const Dock &entry, const base::Rect &output) const;
    bool isOverFullscreen(const Window *dock, const Dock &entry, const base::Rect &output) const;
    void updateReveal(Window *dock, Dock &entry);
    bool isLaunchpadShowing(const base::Rect &output) const;
    void applyAutoHideState(Window *dock, Dock &entry, DockAutoHideState state);
    void slideTo(Window *dock, Dock &entry, const base::Point &offset);
    void present(Window *dock, const Dock &entry);
    void requestFrame();

    WindowManager *m_manager = nullptr;
    DockAutoHideBehavior m_behavior = DockAutoHideBehavior::Never;
    bool m_sessionStarting = false;
    bool m_sessionEnding = false;
    std::map<Window *, std::unique_ptr<Dock>> m_docks;
    std::optional<base::PointF> m_pointer;
    // Upward push gathered against the top over a fullscreen window, and whether it was enough.
    double m_topPush = 0;
    std::chrono::steady_clock::time_point m_lastTopPush;
    bool m_pushRevealed = false;
    double m_overviewProgress = 0;
    std::vector<base::Subscription> m_managerSignals;
};

} // namespace properdesktop::wm

#endif // PROPERDESKTOP_WM_DOCK_LAYOUT_MANAGER_H_
