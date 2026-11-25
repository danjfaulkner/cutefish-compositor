// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modelled on chromium/src/ash/wm/lock_state_controller.cc, which drops capture,
// gestures and the active window before the lock screen is shown, and on
// chromium/src/ash/login/ui/lock_screen.h, whose saved_clipboard_ gives the lock
// screen a clipboard of its own.
// Revision: 7acd40ddefff49c9d36144999aafdbb674dbd07a

#ifndef PROPERDESKTOP_SESSION_SESSION_TEARDOWN_H_
#define PROPERDESKTOP_SESSION_SESSION_TEARDOWN_H_

#include <functional>

#include <cstdint>

#include "session/session_controller.h"

namespace properdesktop::input {
class Seat;
}

namespace properdesktop::wm {
class WindowCycleController;
class WindowManager;
}

namespace properdesktop::session {

// A grab, a popup grab, a drag and a cursor surface outlive the hit test that
// granted them, so none may survive into a blocked session.
class SessionTeardown : public SessionController::Delegate
{
public:
    SessionTeardown(input::Seat *seat, wm::WindowManager *windowManager);

    // Without it only the hardware cursor plane is not refreshed.
    void setCursorReset(std::function<void()> reset);

    void setCaptureAbort(std::function<void()> abort);

    // Cancelled with the block: the modifiers that would complete it are let
    // go of behind the lock screen.
    void setWindowCycle(wm::WindowCycleController *windowCycle);

    void onSessionBlocked() override;
    void onSessionUnblocked() override;

private:
    void clearCursor();

    input::Seat *m_seat = nullptr;
    wm::WindowManager *m_windowManager = nullptr;
    wm::WindowCycleController *m_windowCycle = nullptr;
    // By id, which a window closing meanwhile cannot leave dangling.
    uint64_t m_activeBeforeBlock = 0;
    std::function<void()> m_cursorReset;
    std::function<void()> m_captureAbort;
};

} // namespace properdesktop::session

#endif // PROPERDESKTOP_SESSION_SESSION_TEARDOWN_H_
