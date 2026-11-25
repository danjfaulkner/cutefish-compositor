find_package(ProperDesktopClients REQUIRED COMPONENTS appearance)
add_library(properdesktop-shell-wallpaper STATIC
    src/shell/wallpaper/bar_backdrop.cpp
    src/shell/wallpaper/bar_backdrop.h
    src/shell/wallpaper/wallpaper.cpp
    src/shell/wallpaper/wallpaper.h
    src/shell/wallpaper/wallpaper_cache.cpp
    src/shell/wallpaper/wallpaper_cache.h
)
target_link_libraries(properdesktop-shell-wallpaper PUBLIC properdesktop-shell ProperDesktop::AppearanceClient)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-wallpaper)

add_library(properdesktop-shell-osd STATIC
    src/shell/osd/level_track.cpp
    src/shell/osd/level_track.h
    src/shell/osd/osd.cpp
    src/shell/osd/osd.h
    src/shell/osd/osd_levels.cpp
)
target_link_libraries(properdesktop-shell-osd PUBLIC properdesktop-shell properdesktop-shell-menubar
    ProperDesktop::AudioClient)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-osd)

add_library(properdesktop-shell-dock-view STATIC
    src/shell/dock/dock_icon.cpp
    src/shell/dock/dock_icon.h
    src/shell/dock/dock_magnification.cpp
    src/shell/dock/dock_magnification.h
    src/shell/dock/dock_metrics.h
    src/shell/dock/dock_presentation.cpp
    src/shell/dock/dock_presentation.h
    src/shell/dock/dock_tooltip.cpp
    src/shell/dock/dock_tooltip.h
    src/shell/dock/dock_window.cpp
    src/shell/dock/dock_window.h
)
target_link_libraries(properdesktop-shell-dock-view PUBLIC properdesktop-shell properdesktop-shell-dock)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-dock-view)

add_library(properdesktop-shell-clipboard-view STATIC
    src/shell/clipboard/clipboard_picker.cpp
    src/shell/clipboard/clipboard_picker.h
    src/shell/clipboard/clipboard_picker_view.cpp
    src/shell/clipboard/clipboard_picker_view.h
)
target_link_libraries(properdesktop-shell-clipboard-view PUBLIC properdesktop-shell properdesktop-shell-clipboard)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-clipboard-view)

add_library(properdesktop-shell-notresponding STATIC
    src/shell/notresponding/not_responding_sheets.cpp
    src/shell/notresponding/not_responding_sheets.h
)
target_link_libraries(properdesktop-shell-notresponding PUBLIC properdesktop-shell)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-notresponding)
add_library(properdesktop-shell-screenshot STATIC
    src/shell/screenshot/capture_clipboard.cpp
    src/shell/screenshot/capture_clipboard.h
    src/shell/screenshot/capture_notifier.cpp
    src/shell/screenshot/capture_notifier.h
    src/shell/screenshot/capture_overlay.cpp
    src/shell/screenshot/capture_overlay.h
    src/shell/screenshot/capture_settings.cpp
    src/shell/screenshot/capture_settings.h
    src/shell/screenshot/capture_toolbar.cpp
    src/shell/screenshot/capture_toolbar.h
    src/shell/screenshot/screen_capture.cpp
    src/shell/screenshot/screen_capture.h
    src/shell/screenshot/screen_grabber.cpp
    src/shell/screenshot/screen_grabber.h
    src/shell/screenshot/screen_recording.cpp
    src/shell/screenshot/screen_recording.h
)
target_link_libraries(properdesktop-shell-screenshot PUBLIC properdesktop-shell properdesktop-shell-common
    properdesktop-wayland properdesktop-wayland-private-shell ProperDesktop::AudioClient)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-screenshot)
find_package(ProperDesktopClients REQUIRED COMPONENTS networkmanager bluez)
add_library(properdesktop-shell-secrets STATIC
    src/shell/secrets/bluetooth_pairing_dialog.cpp
    src/shell/secrets/bluetooth_pairing_dialog.h
    src/shell/secrets/network_dialogs.cpp
    src/shell/secrets/network_dialogs.h
    src/shell/secrets/network_secret_agent.cpp
    src/shell/secrets/network_secret_agent.h
    src/shell/secrets/secret_dialog.cpp
    src/shell/secrets/secret_dialog.h
    src/shell/secrets/secret_prompts.cpp
    src/shell/secrets/secret_prompts.h
    src/shell/secrets/secret_store.cpp
    src/shell/secrets/secret_store.h
    src/shell/secrets/vpn_secret_fields.cpp
    src/shell/secrets/vpn_secret_fields.h
)
target_link_libraries(properdesktop-shell-secrets PUBLIC properdesktop-shell
    ProperDesktop::NetworkManagerClient ProperDesktop::BluezClient)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-secrets)
add_library(properdesktop-shell-notification-view STATIC
    src/shell/notification/notification_card.cpp
    src/shell/notification/notification_card.h
    src/shell/notification/notification_centre.cpp
    src/shell/notification/notification_centre.h
    src/shell/notification/notification_controls.cpp
    src/shell/notification/notification_controls.h
    src/shell/notification/notification_motion.cpp
    src/shell/notification/notification_motion.h
    src/shell/notification/notification_popups.cpp
    src/shell/notification/notification_popups.h
    src/shell/notification/notification_ui.cpp
    src/shell/notification/notification_ui.h
)
target_link_libraries(properdesktop-shell-notification-view PUBLIC properdesktop-shell properdesktop-shell-notification)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-notification-view)
add_library(properdesktop-shell-launchpad-view STATIC
    src/shell/launchpad/launchpad_cell.cpp
    src/shell/launchpad/launchpad_cell.h
    src/shell/launchpad/launchpad_folder_view.cpp
    src/shell/launchpad/launchpad_folder_view.h
    src/shell/launchpad/launchpad_metrics.cpp
    src/shell/launchpad/launchpad_metrics.h
    src/shell/launchpad/launchpad_page_indicator.cpp
    src/shell/launchpad/launchpad_page_indicator.h
    src/shell/launchpad/launchpad_search_field.cpp
    src/shell/launchpad/launchpad_search_field.h
    src/shell/launchpad/launchpad_tween.h
    src/shell/launchpad/launchpad_view.cpp
    src/shell/launchpad/launchpad_view.h
    src/shell/launchpad/launchpad_wallpaper.cpp
    src/shell/launchpad/launchpad_wallpaper.h
    src/shell/launchpad/launchpad_window.cpp
    src/shell/launchpad/launchpad_window.h
)
target_link_libraries(properdesktop-shell-launchpad-view PUBLIC properdesktop-shell properdesktop-shell-launchpad
    properdesktop-shell-wallpaper)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-launchpad-view)
find_package(ProperDesktopClients REQUIRED COMPONENTS networkmanager bluez audio)
add_library(properdesktop-shell-controlcenter STATIC
    src/shell/controlcenter/control_center.cpp
    src/shell/controlcenter/control_center.h
    src/shell/controlcenter/control_center_services.h
    src/shell/controlcenter/control_center_view.cpp
    src/shell/controlcenter/control_center_view.h
    src/shell/controlcenter/level_slider.cpp
    src/shell/controlcenter/level_slider.h
    src/shell/controlcenter/panel_style.cpp
    src/shell/controlcenter/panel_style.h
    src/shell/controlcenter/power_dialog.cpp
    src/shell/controlcenter/power_dialog.h
    src/shell/controlcenter/session_services.cpp
    src/shell/controlcenter/session_services.h
    src/shell/controlcenter/status_panel.cpp
    src/shell/controlcenter/status_panel.h
    src/shell/controlcenter/tiles.cpp
    src/shell/controlcenter/tiles.h
    src/shell/menubar/menu_bar_panels.h
)
target_link_libraries(properdesktop-shell-controlcenter PUBLIC properdesktop-shell properdesktop-shell-osd
    properdesktop-shell-menubar properdesktop-shell-notification ProperDesktop::NetworkManagerClient
    ProperDesktop::BluezClient ProperDesktop::AudioClient)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-controlcenter)
add_library(properdesktop-shell-bar-items STATIC
    src/shell/menubar/bar_item.cpp
    src/shell/menubar/bar_item.h
    src/shell/menubar/menu_bar_panels.h
    src/shell/menubar/status_icon.cpp
    src/shell/menubar/status_icon.h
)
target_link_libraries(properdesktop-shell-bar-items PUBLIC properdesktop-shell properdesktop-shell-menubar)
find_package(ProperDesktopClients REQUIRED COMPONENTS appearance)
add_library(properdesktop-shell-menubar-view STATIC
    src/shell/menubar/menu_popup.cpp
    src/shell/menubar/menu_popup.h
    src/shell/menubar/device_usage.cpp
    src/shell/menubar/device_usage.h
    src/shell/menubar/menu_bar_backdrop.cpp
    src/shell/menubar/menu_bar_backdrop.h
    src/shell/menubar/menu_bar_app_menus.h
    src/shell/menubar/menu_bar_panels.h
    src/shell/menubar/menu_bar_window.cpp
    src/shell/menubar/menu_bar_window.h
    src/shell/menubar/status_items_view.cpp
    src/shell/menubar/status_items_view.h
    src/shell/menubar/system_tray_view.cpp
    src/shell/menubar/system_tray_view.h
)
target_link_libraries(properdesktop-shell-menubar-view PUBLIC properdesktop-shell properdesktop-shell-bar-items
    properdesktop-shell-menubar properdesktop-shell-wallpaper ProperDesktop::AudioClient)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-menubar-view)
include(src/shell/menubar/app_menus.cmake)
