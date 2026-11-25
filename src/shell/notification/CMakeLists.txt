add_library(properdesktop-shell-notification STATIC
    notification.cpp
    notification.h
    notification_images.cpp
    notification_images.h
    notification_models.cpp
    notification_models.h
    notification_server.cpp
    notification_server.h
    notification_settings.cpp
    notification_settings.h
    notifications.cpp
    notifications.h
)
target_link_libraries(properdesktop-shell-notification PUBLIC properdesktop-shell-common)
