// Copyright 2026 danjfaulkner
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/desks/fullscreen_desks.h"

#include <chrono>
#include <set>

#include "base/auto_reset.h"
#include "core/logging.h"
#include "wm/desks/desk.h"
#include "wm/desks/desk_animation.h"
#include "wm/window.h"
#include "wm/window_manager.h"
#include "wm/window_state.h"
#include "wm/wm_event.h"

namespace cutefish::wm {

FullscreenDesks::FullscreenDesks(WindowManager *manager)
    : m_manager(manager)
{
    if (DesksController *controller = desks()) {
        controller->addObserver(this);
        // A window opened from a fullscreen desk opens on the desk that window came from.
        controller->setNewWindowDeskOverride([this](Window *, Desk *desk) -> Desk * {
            return isFullscreenDesk(desk) ? originOf(desk) : nullptr;
        });
    }
}

FullscreenDesks::~FullscreenDesks()
{
    if (DesksController *controller = desks()) {
        controller->setNewWindowDeskOverride(nullptr);
        controller->removeObserver(this);
    }
    for (const auto &[window, entry] : m_entries)
        window->removeObserver(this);
}

DesksController *FullscreenDesks::desks() const
{
    return m_manager ? m_manager->desks() : nullptr;
}

bool FullscreenDesks::canEnter(const Window *window) const
{
    return refusal(window).empty();
}

std::string FullscreenDesks::refusal(const Window *window) const
{
    if (!window)
        return "no window";
    if (!desks())
        return "no desks";
    if (!m_manager->hasWindow(window))
        return "not managed";
    if (window->role() != WindowRole::Normal || window->isShellWindow())
        return "not an application window";
    if (window->isPopup())
        return "popup";
    if (window->transientParent())
        return "has a transient parent";
    if (!window->isResizable())
        return "not resizable";
    if (window->stateType() == WindowStateType::Fullscreen)
        return "already fullscreen";
    if (owns(window))
        return "already on its own desk";
    return {};
}

bool FullscreenDesks::owns(const Window *window) const
{
    return m_entries.count(const_cast<Window *>(window)) != 0;
}

bool FullscreenDesks::isFullscreenDesk(const Desk *desk) const
{
    return desk && desk->isFullscreenDesk();
}

Desk *FullscreenDesks::originOf(const Desk *desk) const
{
    if (!desk)
        return nullptr;
    for (const auto &[window, entry] : m_entries) {
        if (entry.desk == desk->id())
            return originFor(entry);
    }
    return nullptr;
}

Desk *FullscreenDesks::originFor(const Entry &entry) const
{
    DesksController *controller = desks();
    if (Desk *origin = controller->deskById(entry.origin))
        return origin;
    // The desk it came from is gone: the one before the fullscreen desk stands in.
    const int index = controller->deskIndex(controller->deskById(entry.desk));
    for (int i = index - 1; i >= 0; --i) {
        if (!controller->deskAt(i)->isFullscreenDesk())
            return controller->deskAt(i);
    }
    for (const auto &desk : controller->desks()) {
        if (!desk->isFullscreenDesk())
            return desk.get();
    }
    return nullptr;
}

std::string FullscreenDesks::nameFor(const Window *window) const
{
    std::string base = m_nameResolver ? m_nameResolver(window) : std::string();
    if (base.empty())
        base = window->title();
    if (base.empty())
        base = window->applicationId();
    if (base.empty())
        base = "Full Screen";

    std::set<std::string> taken;
    for (const auto &desk : desks()->desks())
        taken.insert(desk->name());
    if (!taken.count(base))
        return base;
    for (int n = 2;; ++n) {
        const std::string name = base + " (" + std::to_string(n) + ")";
        if (!taken.count(name))
            return name;
    }
}

bool FullscreenDesks::toggle(Window *window)
{
    if (!window || !m_manager->hasWindow(window))
        return false;
    if (owns(window)) {
        exit(window);
        return true;
    }
    if (window->stateType() == WindowStateType::Fullscreen) {
        // Fullscreen in place (the client's own): just leave it.
        window->state()->onEvent(WMEvent(WMEventType::ToggleFullscreen));
        return true;
    }
    if (const std::string why = refusal(window); !why.empty()) {
        CF_WARNING(cfCompositor) << "fullscreen desk refused for" << window->applicationId() << ":" << why;
        return false;
    }
    return enter(window);
}

bool FullscreenDesks::enter(Window *window)
{
    DesksController *controller = desks();
    if (!canEnter(window))
        return false;
    if (DeskAnimationBase *animation = controller->animation())
        animation->finishNow();

    if (!controller->canCreateDesks()) {
        // Every desk is taken: fullscreen in place rather than nothing.
        CF_DEBUG(cfCompositor) << "no desk left for fullscreen, staying in place";
        window->state()->onEvent(WMEvent(WMEventType::Fullscreen));
        changed.emit();
        return true;
    }

    const base::AutoReset changing(m_changing, true);
    if (controller->isVisibleOnAllDesks(window))
        controller->setVisibleOnAllDesks(window, false);
    Desk *origin = controller->deskOf(window);
    if (!origin)
        origin = controller->activeDesk();

    const std::string name = nameFor(window);
    Desk *desk = controller->newDesk(DesksCreationRemovalSource::Api);
    if (!desk)
        return false;
    desk->setFullscreenDesk(true);
    controller->setDeskName(desk, name);
    controller->reorderDesk(controller->deskIndex(desk), controller->deskIndex(origin) + 1);

    m_entries[window] = {desk->id(), origin->id()};
    window->addObserver(this);
    if (origin == controller->activeDesk()) {
        // On every desk while the desks switch, so the window is never hidden and
        // activation never passes to another window on the way; then it belongs to the
        // fullscreen desk, which is active by then. The full slide of a desk switch by
        // keyboard, not the quick fade an API switch gets.
        controller->setVisibleOnAllDesks(window, true);
        controller->activateDesk(desk, DesksSwitchSource::DeskSwitchShortcut);
        controller->setVisibleOnAllDesks(window, false);
        if (controller->deskOf(window) != desk)
            controller->moveWindowFromActiveDeskTo(window, desk);
    } else {
        controller->sendToDeskAtIndex(window, controller->deskIndex(desk));
        controller->activateDesk(desk, DesksSwitchSource::DeskSwitchShortcut);
    }
    window->state()->onEvent(WMEvent(WMEventType::Fullscreen));
    changed.emit();
    return true;
}

void FullscreenDesks::exit(Window *window)
{
    const auto it = m_entries.find(window);
    if (it == m_entries.end())
        return;
    const Entry entry = it->second;
    forget(window);

    DesksController *controller = desks();
    Desk *desk = controller->deskById(entry.desk);
    Desk *origin = originFor(entry);
    {
        const base::AutoReset changing(m_changing, true);
        if (window->stateType() == WindowStateType::Fullscreen)
            window->state()->onEvent(WMEvent(WMEventType::Restore));
        // Off screen, the window goes straight back; on screen the desk's removal takes it.
        if (desk && origin && !desk->isActive() && controller->deskOf(window) == desk
            && origin != controller->activeDesk())
            controller->sendToDeskAtIndex(window, controller->deskIndex(origin));
    }
    removeDeskLater(entry.desk, window->id());
    changed.emit();
}

void FullscreenDesks::forget(Window *window)
{
    if (m_entries.erase(window))
        window->removeObserver(this);
}

void FullscreenDesks::removeDeskLater(uint32_t deskId, uint64_t activateWindow)
{
    // Later, so a window being destroyed has left the desk, and nothing removes the desk
    // from inside a desk or window notification.
    m_manager->runLater(std::chrono::milliseconds(0), [this, deskId, activateWindow] {
        DesksController *controller = desks();
        Desk *desk = controller->deskById(deskId);
        if (!desk || !controller->canRemoveDesks())
            return;
        for (const auto &[window, entry] : m_entries) {
            if (entry.desk == deskId)
                return;
        }
        // Removing the active desk slides to the one before it, so that is where it came from.
        Entry entry {deskId, 0};
        if (Desk *origin = originFor(entry); origin && desk->isActive() && !controller->animation()) {
            const int from = controller->deskIndex(desk);
            const int originIndex = controller->deskIndex(origin);
            if (from != originIndex + 1)
                controller->reorderDesk(from, originIndex < from ? originIndex + 1 : originIndex);
        }
        // The removal moves the window to the desk it came from once the slide lands, and
        // the desk switched to during the slide does not hand out activation.
        if (activateWindow)
            m_activateOnRemoval[deskId] = activateWindow;
        controller->removeDesk(desk, DesksCreationRemovalSource::Api);
    });
}

void FullscreenDesks::onWindowRemoving(Window *window)
{
    const auto it = m_entries.find(window);
    if (it == m_entries.end())
        return;
    const uint32_t desk = it->second.desk;
    forget(window);
    removeDeskLater(desk, 0);
    changed.emit();
}

void FullscreenDesks::onDeskRemoved(const Desk *desk)
{
    if (const auto pending = m_activateOnRemoval.find(desk->id());
        pending != m_activateOnRemoval.end()) {
        const uint64_t id = pending->second;
        m_activateOnRemoval.erase(pending);
        m_manager->runLater(std::chrono::milliseconds(0), [this, id] {
            Window *window = m_manager->windowById(id);
            if (window && window->isVisible() && !window->isActive())
                window->activate();
        });
    }
    // Removed from elsewhere (overview, shortcut): its window has moved on, so it leaves
    // fullscreen too.
    for (auto it = m_entries.begin(); it != m_entries.end();) {
        if (it->second.desk != desk->id()) {
            ++it;
            continue;
        }
        Window *window = it->first;
        it = m_entries.erase(it);
        window->removeObserver(this);
        const base::AutoReset changing(m_changing, true);
        if (window->stateType() == WindowStateType::Fullscreen)
            window->state()->onEvent(WMEvent(WMEventType::Restore));
        changed.emit();
    }
}

void FullscreenDesks::onWindowDeskChanged(Window *window)
{
    if (m_changing)
        return;
    const auto it = m_entries.find(window);
    if (it == m_entries.end() || desks()->deskOf(window) && desks()->deskOf(window)->id() == it->second.desk)
        return;
    // Moved off its fullscreen desk: it leaves fullscreen there and the desk goes.
    const uint32_t desk = it->second.desk;
    forget(window);
    {
        const base::AutoReset changing(m_changing, true);
        if (window->stateType() == WindowStateType::Fullscreen)
            window->state()->onEvent(WMEvent(WMEventType::Restore));
    }
    removeDeskLater(desk, 0);
    changed.emit();
}

void FullscreenDesks::onWindowStateTypeChanged(Window *window, WindowStateType)
{
    if (m_changing || !owns(window) || window->stateType() == WindowStateType::Fullscreen)
        return;
    // It left fullscreen some other way (the client, a shortcut, minimizing): it goes home.
    const uint64_t id = window->id();
    m_manager->runLater(std::chrono::milliseconds(0), [this, id] {
        if (Window *window = m_manager->windowById(id); window && owns(window))
            exit(window);
    });
}

} // namespace cutefish::wm
