add_library(properdesktop-shell-app-menus STATIC
    src/shell/menubar/app_menus_model.cpp
    src/shell/menubar/app_menus_model.h
    src/shell/menubar/app_menus_view.cpp
    src/shell/menubar/app_menus_view.h
)
target_link_libraries(properdesktop-shell-app-menus PUBLIC properdesktop-shell properdesktop-shell-menubar
    properdesktop-shell-menubar-view)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-shell-app-menus)
