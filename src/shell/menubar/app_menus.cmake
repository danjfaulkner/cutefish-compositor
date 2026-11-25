find_package(ProperDesktopClients REQUIRED COMPONENTS accounts)

add_library(properdesktop-shell-app-menus STATIC
    src/shell/menubar/app_menus_model.cpp
    src/shell/menubar/app_menus_model.h
    src/shell/menubar/app_menus_view.cpp
    src/shell/menubar/app_menus_view.h
)
target_link_libraries(properdesktop-shell-app-menus PUBLIC properdesktop-shell properdesktop-shell-menubar
    properdesktop-shell-menubar-view ProperDesktop::AccountsClient)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-app-menus)
