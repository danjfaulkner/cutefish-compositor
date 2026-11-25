add_library(properdesktop-shell-dock STATIC
    dock.cpp
    dock.h
    dock_model.cpp
    dock_model.h
    dock_settings.cpp
    dock_settings.h
    minimized_windows.cpp
    minimized_windows.h
    trash_monitor.cpp
    trash_monitor.h
)
target_link_libraries(properdesktop-shell-dock PUBLIC properdesktop-shell-common)
