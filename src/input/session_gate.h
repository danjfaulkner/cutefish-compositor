// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Modelled on chromium/src/ash/wm/event_client_impl.cc
// (EventClientImpl::GetCanProcessEventsWithinSubtree) and
// chromium/src/ash/wm/window_util.cc (GetBlockingContainersForRoot).
// Revision: 7acd40ddefff49c9d36144999aafdbb674dbd07a

#ifndef PROPERDESKTOP_INPUT_SESSION_GATE_H_
#define PROPERDESKTOP_INPUT_SESSION_GATE_H_

namespace properdesktop::compositor {
class Surface;
}

namespace properdesktop::scene {
class Node;
}

namespace properdesktop::input {

class EventSink;

class SessionGate
{
public:
    virtual ~SessionGate() = default;

    virtual bool restrictsInput() const = 0;

    virtual EventSink *secureInput() const { return nullptr; }

    // Null means nothing is targetable.
    virtual scene::Node *inputRoot() const = 0;

    virtual bool allowsFocus(const compositor::Surface *surface) const = 0;

    // Windows never live in the secure container, so a blocked session refuses every one.
    virtual bool allowsActivation() const = 0;
};

} // namespace properdesktop::input

#endif // PROPERDESKTOP_INPUT_SESSION_GATE_H_
