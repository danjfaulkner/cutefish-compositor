add_library(properdesktop-shell-launchpad STATIC
    launchpad.cpp
    launchpad.h
    launchpad_model.cpp
    launchpad_model.h
)
target_link_libraries(properdesktop-shell-launchpad PUBLIC properdesktop-shell-dock)
