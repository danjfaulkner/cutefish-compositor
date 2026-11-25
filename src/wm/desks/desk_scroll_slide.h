// Copyright 2026 danjfaulkner
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_WM_DESKS_DESK_SCROLL_SLIDE_H_
#define PROPERDESKTOP_WM_DESKS_DESK_SCROLL_SLIDE_H_

#include "input/types.h"
#include "wm/gestures/wm_gesture_handler.h"

namespace properdesktop::wm {

class DesksController;

// Smooth scrolling (two fingers on a touchpad) sliding the desks along with the fingers, as the
// three-finger swipe does, until the fingers lift; then the slide settles on the nearest desk, or
// the one a flick was heading for. Scrolling down (or towards you) brings the next desk in, as the
// wheel does.
class DeskScrollSlide
{
public:
    // The same finger travel per desk as the swipe, in scroll units.
    static constexpr double kScrollPerDesk =
            WmGestureHandler::kSwipeLengthPerDeskMm / WmGestureHandler::kMmPerUnacceleratedUnit;

    explicit DeskScrollSlide(DesksController *desks)
        : m_desks(desks)
    {
    }

    bool sliding() const { return m_sliding; }
    // Starts the slide or carries it on. False when it cannot start (a single desk, or activation is
    // not allowed now): the scroll is then someone else's.
    bool scroll(input::Timestamp timestamp, double scroll);
    // The fingers lifted.
    void end(input::Timestamp timestamp);

private:
    DesksController *m_desks;
    bool m_sliding = false;
};

} // namespace properdesktop::wm

#endif // PROPERDESKTOP_WM_DESKS_DESK_SCROLL_SLIDE_H_
