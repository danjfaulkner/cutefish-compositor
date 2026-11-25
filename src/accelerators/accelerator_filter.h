// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.chromium file.
//
// Upstream: chromium/src/ui/wm/core/accelerator_filter.h and
//           chromium/src/ash/accelerators/pre_target_accelerator_handler.h
// Revision: 8abbac30b9cf2fe976422c58c000c94685648b65

#ifndef PROPERDESKTOP_ACCELERATORS_ACCELERATOR_FILTER_H_
#define PROPERDESKTOP_ACCELERATORS_ACCELERATOR_FILTER_H_

#include <chrono>
#include <optional>

#include "accelerators/accelerator.h"
#include "base/geometry.h"
#include "base/timer.h"
#include "input/event_sink.h"
#include "input/types.h"
#include "wm/desks/desk_scroll_slide.h"

namespace properdesktop::input {
class XkbTracker;
}

namespace properdesktop::wm {
class WindowCycleController;
class WindowManager;
} // namespace properdesktop::wm

namespace properdesktop::accelerators {

class AcceleratorController;

// Every layout but the US ones pins punctuation shortcuts by position.
bool usesPositionalShortcuts(const input::XkbTracker &xkb);

class AcceleratorFilter : public input::EventSink
{

public:
    // After Super and the wheel go to another desk, the wheel does nothing for this long (GNOME's
    // SCROLL_TIMEOUT_TIME).
    static constexpr std::chrono::milliseconds kDeskScrollPause {150};

    // `next` receives whatever is not consumed here.
    AcceleratorFilter(AcceleratorController *controller, const input::XkbTracker *xkb,
                      wm::WindowManager *windowManager, wm::WindowCycleController *windowCycle,
                      input::EventSink *next);
    ~AcceleratorFilter() override;

    // Under the current xkb state, which the seat has already advanced for this key.
    Accelerator acceleratorForKey(uint32_t evdevCode, bool pressed) const;

    // libinput sends no repeats, so a held repeatable accelerator is performed
    // again at the keyboard's own rate, the way the seat repeats for clients.
    void setKeyRepeatSettings(const input::KeyRepeatSettings &settings);

    bool onPointerMotion(input::Timestamp timestamp, const base::PointF &location) override;
    bool onPointerButton(input::Timestamp timestamp, const base::PointF &location,
                         uint32_t evdevButton, bool pressed) override;
    bool onPointerAxis(input::Timestamp timestamp, const base::PointF &location,
                       const base::PointF &offset, bool discrete) override;
    bool onPointerAxisStop(input::Timestamp timestamp, const base::PointF &location) override;
    bool onKey(input::Timestamp timestamp, uint32_t evdevCode, bool pressed) override;
    bool onSecureKey(input::Timestamp timestamp, uint32_t evdevCode, bool pressed) override;
    // Touch passes straight through.
    bool onTouchDown(input::Timestamp timestamp, int32_t id, const base::PointF &location) override;
    bool onTouchMotion(input::Timestamp timestamp, int32_t id, const base::PointF &location) override;
    bool onTouchUp(input::Timestamp timestamp, int32_t id) override;
    void onTouchCancel(input::Timestamp timestamp) override;

private:
    bool processAccelerator(const Accelerator &accelerator);
    bool shouldProcessAcceleratorNow(const Accelerator &accelerator) const;
    bool canConsumeSystemKeys() const;

    void startRepeat(const Accelerator &accelerator);
    void stopRepeat();
    void onRepeat();

    // Super and the scroll wheel go through the desks as on GNOME: a desk per whole wheel click, then
    // a pause of kDeskScrollPause in which the wheel does nothing, so a flick of the wheel moves one
    // desk rather than racing through them. Super and touchpad scrolling slide the desks along with
    // the fingers, as the three-finger swipe does, and settle on the nearest when they lift.
    bool scrollThroughDesks(input::Timestamp timestamp, const base::PointF &offset, bool discrete);
    // Smooth scrolling with Super: starts the slide, or carries it on until the fingers lift.
    bool slideDesks(input::Timestamp timestamp, double scroll);
    wm::DeskScrollSlide *deskSlide();

    AcceleratorController *m_controller = nullptr;
    const input::XkbTracker *m_xkb = nullptr;
    // The wheel added up towards a whole click: a high-resolution wheel reports a click in parts.
    double m_deskScroll = 0;
    // When the wheel last went to another desk.
    std::optional<input::Timestamp> m_deskScrolled;
    // Super and touchpad scrolling sliding the desks, until the fingers lift.
    std::optional<wm::DeskScrollSlide> m_deskSlide;
    // What the accelerator that started the window cycle held besides Shift.
    uint32_t m_cycleModifiers = 0;
    wm::WindowManager *m_windowManager = nullptr;
    wm::WindowCycleController *m_windowCycle = nullptr;
    input::EventSink *m_next = nullptr;

    input::KeyRepeatSettings m_repeatSettings;
    base::Timer m_repeatTimer;
    Accelerator m_repeatAccelerator;
};

} // namespace properdesktop::accelerators

#endif // PROPERDESKTOP_ACCELERATORS_ACCELERATOR_FILTER_H_
