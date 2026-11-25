// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_MENUBAR_POWER_ACTIONS_H_
#define PROPERDESKTOP_SHELL_MENUBAR_POWER_ACTIONS_H_

#include "base/dbus.h"
#include "base/signal.h"
#include "base/timer.h"
#include "shell/common/pending_calls.h"

namespace properdesktop::shell {

class PowerActions
{
public:
    enum Action {
        None,
        Logout,
        Reboot,
        Shutdown,
    };

    explicit PowerActions(base::dbus::Connection *bus = base::dbus::Connection::session());

    // None while the dialog is not up.
    Action pending() const { return m_pending; }

    void request(Action action);
    // The pending action clears on the next turn of the loop: the dialog's
    // answer is still on the stack.
    void confirm();
    void cancel();

    void logout();
    void reboot();
    void shutdown();
    void suspend();
    void lockScreen();

    base::Signal<> pendingChanged;
    base::Signal<> requested;

private:
    void setPending(Action action);
    void callSession(const char *method);

    base::dbus::Proxy m_session;
    base::dbus::Proxy m_screenSaver;
    PendingCalls m_calls;
    Action m_pending = None;
    base::Timer m_clear;
};

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_MENUBAR_POWER_ACTIONS_H_
