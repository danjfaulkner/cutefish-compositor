// Copyright 2026 Reion Wong <reionwong@gmail.com>
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PROPERDESKTOP_SHELL_MENUBAR_APP_MENUS_VIEW_H_
#define PROPERDESKTOP_SHELL_MENUBAR_APP_MENUS_VIEW_H_

#include <memory>
#include <string>
#include <vector>

#include "base/signal.h"
#include "shell/menubar/app_menus_model.h"
#include "shell/menubar/menu_bar_app_menus.h"
#include "ui/geometry.h"

namespace properdesktop::accounts {
class UserAccount;
}

namespace properdesktop::ui {
class Menu;
}

namespace properdesktop::shell {

class BarItem;
class Shell;

class AppMenusView final : public MenuBarAppMenus
{
public:
    static constexpr double kPopupMargin = 6;

    AppMenusView(Shell *shell, MenuBarAppMenusHost host);
    ~AppMenusView() override;

    void closeMenu() override { close(); }
    void barChanged() override;

    void open(int index, bool keyboard = false);
    void close();
    void step(int step);

    // The entry whose menu is open, or -1.
    int currentIndex() const { return m_currentIndex; }
    ui::Menu *openMenu() const { return m_openMenu; }
    AppMenusModel &model() { return *m_model; }
    BarItem *entry(size_t index) const;
    // The fullscreen button after the menus.
    BarItem *fullscreenButton() const;
    // Empty shows the account's colour.
    void setUserIcon(const std::string &file);

protected:
    void layout() override;

private:
    class Entry;
    class FullscreenButton;

    void rebuildEntries();
    void onMenuClosed();
    // Shown after the menus while the active window can go fullscreen.
    void updateFullscreenButton();

    MenuBarAppMenusHost m_host;
    std::unique_ptr<AppMenusModel> m_model;
    std::unique_ptr<accounts::UserAccount> m_account;
    std::vector<Entry *> m_entries;
    FullscreenButton *m_fullscreen = nullptr;
    std::string m_userIcon;
    ui::Color m_userColor;
    int m_currentIndex = -1;
    ui::Menu *m_openMenu = nullptr;
    bool m_switching = false;
    std::vector<base::Subscription> m_subscriptions;
};

} // namespace properdesktop::shell

#endif // PROPERDESKTOP_SHELL_MENUBAR_APP_MENUS_VIEW_H_
