add_library(properdesktop-shell-clipboard STATIC
    clipboard_history.cpp
    clipboard_history.h
    clipboard_monitor.cpp
    clipboard_monitor.h
    clipboard_store.cpp
    clipboard_store.h
)
target_link_libraries(properdesktop-shell-clipboard PUBLIC properdesktop-shell-common)
