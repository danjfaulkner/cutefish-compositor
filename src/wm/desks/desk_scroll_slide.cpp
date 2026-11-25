// Copyright 2026 danjfaulkner
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/desks/desk_scroll_slide.h"

#include <chrono>

#include "wm/desks/desks_controller.h"

namespace properdesktop::wm {

namespace {

double milliseconds(input::Timestamp timestamp)
{
    return std::chrono::duration<double, std::milli>(timestamp.time_since_epoch()).count();
}

} // namespace

bool DeskScrollSlide::scroll(input::Timestamp timestamp, double scroll)
{
    if (!m_desks || scroll == 0)
        return m_sliding;
    if (!m_sliding) {
        // The swipe's fingers moving left bring the next desk in.
        if (!m_desks->startSwipeAnimation(/*moveLeft=*/scroll < 0))
            return false;
        m_sliding = true;
        m_desks->updateSwipeAnimation(0, milliseconds(timestamp));
    }
    m_desks->updateSwipeAnimation(-scroll / kScrollPerDesk, milliseconds(timestamp));
    return true;
}

void DeskScrollSlide::end(input::Timestamp timestamp)
{
    if (!m_sliding)
        return;
    m_sliding = false;
    if (m_desks)
        m_desks->endSwipeAnimation(milliseconds(timestamp));
}

} // namespace properdesktop::wm
