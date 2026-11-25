// Copyright 2026 danjfaulkner
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wm/desks/fullscreen_desks.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <set>

#include "base/auto_reset.h"
#include "compositor/surface.h"
#include "core/logging.h"
#include "wm/desks/desk.h"
#include "wm/desks/desk_animation.h"
#include "wm/window.h"
#include "wm/window_manager.h"
#include "wm/window_state.h"
#include "wm/wm_event.h"

namespace properdesktop::wm {

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
    m_windowAdded = m_manager->windowAdded.connect([this](Window *window) { onWindowAdded(window); });
}

void FullscreenDesks::setRememberedApplications(std::set<std::string> applications)
{
    m_remembered = std::move(applications);
}

void FullscreenDesks::remember(const std::string &application, bool remembered)
{
    if (application.empty())
        return;
    const bool changed = remembered ? m_remembered.insert(application).second : m_remembered.erase(application) != 0;
    if (changed)
        rememberedChanged.emit();
}

void FullscreenDesks::onWindowAdded(Window *window)
{
    const std::string application = window->applicationId();
    if (!m_remembered.count(application) || m_reopened.count(application))
        return;
    // Once the client has set its state: one restoring fullscreen asks for it before it maps.
    const uint64_t id = window->id();
    m_manager->runLater(std::chrono::milliseconds(0), [this, id, application] {
        Window *window = m_manager->windowById(id);
        if (!window || owns(window) || m_reopened.count(application))
            return;
        const bool adopting = window->stateType() == WindowStateType::Fullscreen;
        if (!adopting && !canEnter(window))
            return;
        m_reopened.insert(application);
        CF_INFO(cfCompositor) << application << "was left fullscreen; reopening it on a desk of its own";
        enterInternal(window, adopting);
    });
}

FullscreenDesks::~FullscreenDesks()
{
    m_windowAdded.reset();
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

bool FullscreenDesks::fills(const Window *window) const
{
    const auto it = m_entries.find(const_cast<Window *>(window));
    return it != m_entries.end() && it->second.fill;
}

bool FullscreenDesks::wantsTitleBar(const Window *window) const
{
    if (!window)
        return false;
    if (owns(window))
        return true;
    const compositor::Surface *root = window->rootSurface();
    return window->stateType() == WindowStateType::Fullscreen && root && root->hasFullscreenBars();
}

bool FullscreenDesks::isPushedByReveal(const Window *window) const
{
    const compositor::Surface *root = window ? window->rootSurface() : nullptr;
    if (root && root->hasFullscreenBars())
        return root->fullscreenBarsPush();
    return owns(window);
}

bool FullscreenDesks::prefersFill(const Window *window) const
{
    std::string id = window->applicationId();
    for (char &c : id)
        c = char(std::tolower(static_cast<unsigned char>(c)));
    if (m_fillApplications.count(id))
        return true;
    // Chromium and the browsers made from it leave fullscreen they did not ask for again.
    for (const char *browser : {"chrom", "helium", "brave", "vivaldi", "msedge", "microsoft-edge",
                                "opera", "thorium", "cromite", "yandex"}) {
        if (id.find(browser) != std::string::npos)
            return true;
    }
    return false;
}

void FullscreenDesks::applyFill(Window *window)
{
    const auto it = m_entries.find(window);
    if (it == m_entries.end())
        return;
    it->second.fill = true;
    const base::AutoReset changing(m_changing, true);
    if (window->stateType() == WindowStateType::Maximized)
        window->state()->onEvent(WMEvent(WMEventType::WorkAreaChanged));
    else
        window->state()->onEvent(WMEvent(WMEventType::Maximize));
    changed.emit();
}

void FullscreenDesks::restoreState(Window *window, const Entry &entry)
{
    const base::AutoReset changing(m_changing, true);
    if (!entry.fill) {
        if (window->stateType() == WindowStateType::Fullscreen)
            window->state()->onEvent(WMEvent(WMEventType::Restore));
        return;
    }
    // Filling, it is maximized, or fullscreen for a video of its own: out of that first.
    if (window->stateType() == WindowStateType::Fullscreen)
        window->state()->onEvent(WMEvent(WMEventType::Restore));
    // Back to the area a maximized window has, or out of maximized.
    if (window->stateType() != WindowStateType::Maximized)
        return;
    if (entry.previous == WindowStateType::Maximized)
        window->state()->onEvent(WMEvent(WMEventType::WorkAreaChanged));
    else
        window->state()->onEvent(WMEvent(WMEventType::Restore));
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
    return enterInternal(window, /*adopting=*/false);
}

bool FullscreenDesks::enterInternal(Window *window, bool adopting)
{
    DesksController *controller = desks();
    if (adopting) {
        if (!window || !controller || !m_manager->hasWindow(window) || owns(window)
            || window->role() != WindowRole::Normal || window->isPopup() || window->transientParent()
            || window->stateType() != WindowStateType::Fullscreen)
            return false;
    } else if (!canEnter(window)) {
        return false;
    }
    if (DeskAnimationBase *animation = controller->animation())
        animation->finishNow();

    if (!controller->canCreateDesks()) {
        // Every desk is taken: fullscreen in place rather than nothing.
        CF_DEBUG(cfCompositor) << "no desk left for fullscreen, staying in place";
        if (!adopting)
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

    CF_INFO(cfCompositor) << window->applicationId() << "goes fullscreen on desk" << desk->id();
    m_entries[window] = {desk->id(), origin->id(), false,
                         adopting ? WindowStateType::Normal : window->stateType(),
                         std::chrono::steady_clock::now()};
    window->addObserver(this);
    remember(window->applicationId(), true);
    // Fullscreen where it is first; its desk is switched to without a slide once it has drawn
    // at the new size, when it covers the output and the switch shows nothing.
    if (adopting)
        ;
    else if (prefersFill(window))
        applyFill(window);
    else
        window->state()->onEvent(WMEvent(WMEventType::Fullscreen));
    const uint64_t id = window->id();
    m_manager->runLater(kSettleTimeout, [this, id] {
        if (Window *window = m_manager->windowById(id); window && owns(window))
            moveToOwnDesk(window);
    });
    changed.emit();
    return true;
}

void FullscreenDesks::handOver(Window *window, Desk *to)
{
    DesksController *controller = desks();
    const base::AutoReset changing(m_changing, true);
    const bool active = window->isActive();
    controller->setVisibleOnAllDesks(window, true);
    controller->activateDeskAtOnce(to, /*updateWindowActivation=*/false);
    controller->setVisibleOnAllDesks(window, false);
    if (controller->deskOf(window) != to)
        controller->sendToDeskAtIndex(window, controller->deskIndex(to));
    if (active && !window->isActive())
        window->activate();
}

void FullscreenDesks::moveToOwnDesk(Window *window)
{
    const auto it = m_entries.find(window);
    if (it == m_entries.end() || it->second.moved)
        return;
    it->second.moved = true;
    DesksController *controller = desks();
    Desk *desk = controller->deskById(it->second.desk);
    if (!desk || controller->deskOf(window) == desk)
        return;
    if (controller->belongsToActiveDesk(window) && window->isVisible()) {
        handOver(window, desk);
    } else {
        // Not on screen any more (another desk, minimized): it moves there quietly.
        const base::AutoReset changing(m_changing, true);
        controller->sendToDeskAtIndex(window, controller->deskIndex(desk));
    }
    changed.emit();
}

void FullscreenDesks::onWindowContentChanged(Window *window)
{
    const auto it = m_entries.find(window);
    if (it == m_entries.end() || it->second.moved)
        return;
    // The client has drawn at the size it was given.
    if (window->contentBounds().size() == window->bounds().size()
        && window->bounds() == m_manager->outputBoundsFor(window)) {
        const uint64_t id = window->id();
        m_manager->runLater(std::chrono::milliseconds(0), [this, id] {
            if (Window *window = m_manager->windowById(id); window && owns(window))
                moveToOwnDesk(window);
        });
    }
}

void FullscreenDesks::exit(Window *window)
{
    exitInternal(window, /*forgetApplication=*/true);
}

void FullscreenDesks::exitInternal(Window *window, bool forgetApplication)
{
    const auto it = m_entries.find(window);
    if (it == m_entries.end())
        return;
    const Entry entry = it->second;
    forget(window);
    CF_INFO(cfCompositor) << window->applicationId() << "leaves fullscreen on desk" << entry.desk;
    if (forgetApplication)
        remember(window->applicationId(), false);

    DesksController *controller = desks();
    Desk *desk = controller->deskById(entry.desk);
    Desk *origin = originFor(entry);
    // Home first, at once and still covering the output; then it leaves fullscreen there.
    if (desk && origin && controller->deskOf(window) == desk) {
        if (desk->isActive() && window->isVisible()) {
            handOver(window, origin);
        } else {
            const base::AutoReset changing(m_changing, true);
            if (origin == controller->activeDesk()) {
                controller->setVisibleOnAllDesks(window, true);
                controller->setVisibleOnAllDesks(window, false);
            } else {
                controller->sendToDeskAtIndex(window, controller->deskIndex(origin));
            }
        }
    }
    restoreState(window, entry);
    removeDeskLater(entry.desk, 0);
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
        // Still the active desk (its window closed): back to the desk it came from at once,
        // with no slide, before it goes.
        Entry entry {deskId, 0};
        if (Desk *origin = originFor(entry); origin && desk->isActive())
            controller->activateDeskAtOnce(origin, /*updateWindowActivation=*/true);
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
        const Entry entry = it->second;
        it = m_entries.erase(it);
        window->removeObserver(this);
        restoreState(window, entry);
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
    const Entry entry = it->second;
    forget(window);
    restoreState(window, entry);
    removeDeskLater(entry.desk, 0);
    changed.emit();
}

void FullscreenDesks::onWindowStateTypeChanged(Window *window, WindowStateType oldType)
{
    const auto it = m_entries.find(window);
    if (m_changing || it == m_entries.end())
        return;
    const Entry &entry = it->second;
    if (window->stateType() == (entry.fill ? WindowStateType::Maximized : WindowStateType::Fullscreen))
        return;
    // Filling its desk, the client's own fullscreen (a video, F11) happens on that desk, and
    // leaving it brings the window back to filling the desk.
    if (entry.fill && window->stateType() == WindowStateType::Fullscreen)
        return;
    const uint64_t id = window->id();
    if (!entry.fill && oldType == WindowStateType::Fullscreen
        && window->stateType() != WindowStateType::Minimized
        && std::chrono::steady_clock::now() - entry.entered < kRefusalWindow) {
        // The client left fullscreen it was just given: it fills the output instead, and so
        // does the rest of this application from now on.
        m_fillApplications.insert([&] {
            std::string app = window->applicationId();
            for (char &c : app)
                c = char(std::tolower(static_cast<unsigned char>(c)));
            return app;
        }());
        CF_INFO(cfCompositor) << window->applicationId() << "refused fullscreen; filling its desk instead";
        m_manager->runLater(std::chrono::milliseconds(0), [this, id] {
            if (Window *window = m_manager->windowById(id); window && owns(window) && !fills(window))
                applyFill(window);
        });
        return;
    }
    // It left fullscreen some other way (the client, a shortcut, minimizing): it goes home. The
    // client may be quitting, so the application is forgotten only if the window stays.
    const std::string application = window->applicationId();
    m_manager->runLater(std::chrono::milliseconds(0), [this, id] {
        if (Window *window = m_manager->windowById(id); window && owns(window))
            exitInternal(window, /*forgetApplication=*/false);
    });
    m_manager->runLater(kQuitGrace, [this, id, application] {
        if (m_manager->windowById(id) && !std::any_of(m_entries.begin(), m_entries.end(), [&](const auto &entry) {
                return entry.first->applicationId() == application;
            }))
            remember(application, false);
    });
}

} // namespace properdesktop::wm
