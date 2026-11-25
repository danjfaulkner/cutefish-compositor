// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Session state follows Ash at revision 7acd40ddefff49c9d36144999aafdbb674dbd07a.

#ifndef PROPERDESKTOP_SESSION_SESSION_STATE_H_
#define PROPERDESKTOP_SESSION_SESSION_STATE_H_

namespace properdesktop::session {

enum class SessionState {
    Active,
    Login,
    Locking,
    Locked,
    Unlocking,
};

// Both transitions remain blocked until secure presentation completes.
constexpr bool isSessionBlocked(SessionState state)
{
    return state != SessionState::Active;
}

const char *toString(SessionState state);

class SessionObserver
{
public:
    virtual ~SessionObserver() = default;

    virtual void onSessionStateChanged(SessionState state) = 0;

    virtual void onSessionOutputsChanged() { }
};

} // namespace properdesktop::session

#endif // PROPERDESKTOP_SESSION_SESSION_STATE_H_
