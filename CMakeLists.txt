cmake_minimum_required(VERSION 3.22)

project(properdesktop-compositor
    VERSION 1.0
    DESCRIPTION "Proper Desktop Wayland compositor and window manager"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)
option(BUILD_TESTING "Build the test suite" OFF)
include(CTest)
# Each test runs apart from the running desktop (tests/isolate.py): its own
# session bus, runtime directory and home, with the account's home out of
# reach.
if(BUILD_TESTING)
    find_program(PROPERDESKTOP_PYTHON python3 REQUIRED)
    find_program(PROPERDESKTOP_DBUS_RUN_SESSION dbus-run-session REQUIRED)
    set(PROPERDESKTOP_TEST_RECORD ${CMAKE_BINARY_DIR}/Testing/isolation)
    set(CMAKE_TEST_LAUNCHER ${PROPERDESKTOP_PYTHON} ${PROJECT_SOURCE_DIR}/tests/isolate.py
        --keep ${PROJECT_SOURCE_DIR} --keep ${CMAKE_BINARY_DIR}
        --record ${PROPERDESKTOP_TEST_RECORD}/writes.log -- ${PROPERDESKTOP_DBUS_RUN_SESSION} --)
endif()

find_package(PkgConfig REQUIRED)
find_package(ProperDesktopToolkit REQUIRED)
pkg_check_modules(TitleText REQUIRED IMPORTED_TARGET harfbuzz freetype2 fontconfig icu-uc icu-i18n)
pkg_check_modules(WaylandServer REQUIRED IMPORTED_TARGET wayland-server>=1.23)
pkg_check_modules(Libseat REQUIRED IMPORTED_TARGET libseat)
pkg_check_modules(Libdrm REQUIRED IMPORTED_TARGET libdrm)
pkg_check_modules(Gbm REQUIRED IMPORTED_TARGET gbm)
pkg_check_modules(Egl REQUIRED IMPORTED_TARGET egl)
pkg_check_modules(GlesV2 REQUIRED IMPORTED_TARGET glesv2)
pkg_check_modules(Xkbcommon REQUIRED IMPORTED_TARGET xkbcommon)
pkg_check_modules(Libinput REQUIRED IMPORTED_TARGET libinput)
pkg_check_modules(Libudev REQUIRED IMPORTED_TARGET libudev)
pkg_check_modules(WaylandProtocols REQUIRED wayland-protocols>=1.39)
pkg_get_variable(WAYLAND_PROTOCOLS_DATADIR wayland-protocols pkgdatadir)
pkg_get_variable(WAYLAND_SCANNER wayland-scanner wayland_scanner)

set(PROPERDESKTOP_PROTOCOL_DIR ${CMAKE_BINARY_DIR}/wayland-protocols)
file(MAKE_DIRECTORY ${PROPERDESKTOP_PROTOCOL_DIR})

function(properdesktop_wayland_protocol name xml)
    set(code ${PROPERDESKTOP_PROTOCOL_DIR}/${name}-protocol.c)
    set(server ${PROPERDESKTOP_PROTOCOL_DIR}/${name}-server-protocol.h)
    set(client ${PROPERDESKTOP_PROTOCOL_DIR}/${name}-client-protocol.h)
    add_custom_command(OUTPUT ${code}
        COMMAND ${WAYLAND_SCANNER} private-code ${xml} ${code}
        DEPENDS ${xml} VERBATIM)
    add_custom_command(OUTPUT ${server}
        COMMAND ${WAYLAND_SCANNER} server-header ${xml} ${server}
        DEPENDS ${xml} VERBATIM)
    add_custom_command(OUTPUT ${client}
        COMMAND ${WAYLAND_SCANNER} client-header ${xml} ${client}
        DEPENDS ${xml} VERBATIM)
    set(${name}_SOURCES ${code} ${server} ${client} PARENT_SCOPE)
endfunction()

properdesktop_wayland_protocol(ext-idle-notify-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/ext-idle-notify/ext-idle-notify-v1.xml)
properdesktop_wayland_protocol(idle-inhibit-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml)
properdesktop_wayland_protocol(pointer-constraints-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml)
properdesktop_wayland_protocol(pointer-gestures-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/pointer-gestures/pointer-gestures-unstable-v1.xml)
properdesktop_wayland_protocol(relative-pointer-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/relative-pointer/relative-pointer-unstable-v1.xml)
properdesktop_wayland_protocol(keyboard-shortcuts-inhibit-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/keyboard-shortcuts-inhibit/keyboard-shortcuts-inhibit-unstable-v1.xml)

properdesktop_wayland_protocol(presentation-time
    ${WAYLAND_PROTOCOLS_DATADIR}/stable/presentation-time/presentation-time.xml)
properdesktop_wayland_protocol(fractional-scale-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/fractional-scale/fractional-scale-v1.xml)
properdesktop_wayland_protocol(xdg-output-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/xdg-output/xdg-output-unstable-v1.xml)
properdesktop_wayland_protocol(viewporter
    ${WAYLAND_PROTOCOLS_DATADIR}/stable/viewporter/viewporter.xml)
properdesktop_wayland_protocol(xdg-activation-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml)
properdesktop_wayland_protocol(xdg-shell
    ${WAYLAND_PROTOCOLS_DATADIR}/stable/xdg-shell/xdg-shell.xml)
properdesktop_wayland_protocol(xdg-decoration
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml)
properdesktop_wayland_protocol(xdg-dialog-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/xdg-dialog/xdg-dialog-v1.xml)
properdesktop_wayland_protocol(xdg-foreign-unstable-v2
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/xdg-foreign/xdg-foreign-unstable-v2.xml)
properdesktop_wayland_protocol(xdg-toplevel-drag-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/xdg-toplevel-drag/xdg-toplevel-drag-v1.xml)

properdesktop_wayland_protocol(linux-dmabuf-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/stable/linux-dmabuf/linux-dmabuf-v1.xml)

properdesktop_wayland_protocol(linux-drm-syncobj-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/linux-drm-syncobj/linux-drm-syncobj-v1.xml)

# properdesktop-toolkit/protocols keeps the client-side copy; the two copies must stay
# byte-identical.
set(PROPERDESKTOP_PRIVATE_PROTOCOL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protocols)
properdesktop_wayland_protocol(properdesktop-shell-v1
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/properdesktop-shell-v1.xml)
properdesktop_wayland_protocol(properdesktop-computer-control-v1
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/properdesktop-computer-control-v1.xml)

properdesktop_wayland_protocol(cursor-shape-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/cursor-shape/cursor-shape-v1.xml)
# cursor-shape's tablet-tool argument references this interface at link time.
# Generate its descriptors only; no tablet global or capability is advertised.
properdesktop_wayland_protocol(tablet-v2
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/tablet/tablet-unstable-v2.xml)
properdesktop_wayland_protocol(properdesktop-window-hints-v1
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/properdesktop-window-hints-v1.xml)
properdesktop_wayland_protocol(properdesktop-surface-blur-v1
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/properdesktop-surface-blur-v1.xml)
properdesktop_wayland_protocol(properdesktop-status-item-v1
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/properdesktop-status-item-v1.xml)
properdesktop_wayland_protocol(properdesktop-main-menu-v1
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/properdesktop-main-menu-v1.xml)
properdesktop_wayland_protocol(properdesktop-window-placement-v1
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/properdesktop-window-placement-v1.xml)
properdesktop_wayland_protocol(ext-data-control-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/ext-data-control/ext-data-control-v1.xml)
properdesktop_wayland_protocol(wlr-data-control-unstable-v1
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-data-control-unstable-v1.xml)
properdesktop_wayland_protocol(primary-selection-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/primary-selection/primary-selection-unstable-v1.xml)
properdesktop_wayland_protocol(appmenu ${CMAKE_CURRENT_SOURCE_DIR}/protocols/appmenu.xml)
properdesktop_wayland_protocol(gtk-shell ${CMAKE_CURRENT_SOURCE_DIR}/protocols/gtk-shell.xml)
properdesktop_wayland_protocol(ext-image-capture-source-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/ext-image-capture-source/ext-image-capture-source-v1.xml)
properdesktop_wayland_protocol(ext-image-copy-capture-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/ext-image-copy-capture/ext-image-copy-capture-v1.xml)
properdesktop_wayland_protocol(ext-foreign-toplevel-list-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/staging/ext-foreign-toplevel-list/ext-foreign-toplevel-list-v1.xml)
properdesktop_wayland_protocol(text-input-unstable-v1
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/text-input/text-input-unstable-v1.xml)
properdesktop_wayland_protocol(text-input-unstable-v3
    ${WAYLAND_PROTOCOLS_DATADIR}/unstable/text-input/text-input-unstable-v3.xml)
properdesktop_wayland_protocol(input-method-unstable-v2
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/input-method-unstable-v2.xml)
properdesktop_wayland_protocol(virtual-keyboard-unstable-v1
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/virtual-keyboard-unstable-v1.xml)
add_library(properdesktop-protocols STATIC
    ${text-input-unstable-v1_SOURCES}
    ${text-input-unstable-v3_SOURCES}
    ${input-method-unstable-v2_SOURCES}
    ${virtual-keyboard-unstable-v1_SOURCES}
    ${ext-idle-notify-v1_SOURCES}
    ${idle-inhibit-unstable-v1_SOURCES}
    ${pointer-constraints-unstable-v1_SOURCES}
    ${pointer-gestures-unstable-v1_SOURCES}
    ${relative-pointer-unstable-v1_SOURCES}
    ${keyboard-shortcuts-inhibit-unstable-v1_SOURCES}
    ${ext-image-capture-source-v1_SOURCES} ${ext-image-copy-capture-v1_SOURCES}
    ${ext-foreign-toplevel-list-v1_SOURCES}
    ${xdg-activation-v1_SOURCES}
    ${appmenu_SOURCES}
    ${gtk-shell_SOURCES}
    ${primary-selection-unstable-v1_SOURCES}
    ${presentation-time_SOURCES}
    ${viewporter_SOURCES}
    ${fractional-scale-v1_SOURCES}
    ${xdg-output-unstable-v1_SOURCES}
    ${ext-data-control-v1_SOURCES} ${wlr-data-control-unstable-v1_SOURCES}
    ${properdesktop-window-hints-v1_SOURCES} ${properdesktop-surface-blur-v1_SOURCES} ${xdg-shell_SOURCES} ${xdg-decoration_SOURCES}
    ${properdesktop-status-item-v1_SOURCES}
    ${properdesktop-main-menu-v1_SOURCES}
    ${properdesktop-window-placement-v1_SOURCES}
    ${xdg-dialog-v1_SOURCES}
    ${xdg-foreign-unstable-v2_SOURCES}
    ${xdg-toplevel-drag-v1_SOURCES}
    ${cursor-shape-v1_SOURCES} ${tablet-v2_SOURCES} ${linux-dmabuf-v1_SOURCES}
    ${linux-drm-syncobj-v1_SOURCES} ${properdesktop-shell-v1_SOURCES}
    ${properdesktop-computer-control-v1_SOURCES})
target_include_directories(properdesktop-protocols PUBLIC ${PROPERDESKTOP_PROTOCOL_DIR})
target_link_libraries(properdesktop-protocols PUBLIC PkgConfig::WaylandServer)
set_target_properties(properdesktop-protocols PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-core STATIC
    src/core/logging.cpp
    src/core/logging.h
    src/core/main_loop.cpp
    src/core/main_loop.h
    src/core/preferences.cpp
    src/core/preferences.h
    src/core/process_environment.cpp
    src/core/process_environment.h
)
target_include_directories(properdesktop-core PUBLIC src)
target_link_libraries(properdesktop-core PUBLIC ProperDesktop::ToolkitBase)
set_target_properties(properdesktop-core PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-display STATIC
    src/display/callback_native_display_delegate.cpp
    src/display/callback_native_display_delegate.h
    src/display/display_configuration_controller.cpp
    src/display/display_configuration_controller.h
    src/display/display_configurator.cpp
    src/display/display_configurator.h
    src/display/display_configuration_validator.cpp
    src/display/display_configuration_validator.h
    src/display/display_identity.cpp
    src/display/display_identity.h
    src/display/display_topology_coordinator.cpp
    src/display/display_topology_coordinator.h
    src/display/display_manager.cpp
    src/display/display_manager.h
    src/display/display_preferences.cpp
    src/display/display_preferences.h
    src/display/display_layout.cpp
    src/display/display_layout.h
    src/display/display_transform.cpp
    src/display/display_transform.h
    src/display/mirror_transform.cpp
    src/display/mirror_transform.h
    src/display/display_types.cpp
    src/display/display_types.h
    src/display/edid.cpp
    src/display/edid.h
    src/display/native_display_delegate.h
    src/display/night_light.cpp
    src/display/night_light.h
    src/display/recommended_scale.cpp
    src/display/recommended_scale.h
)
target_include_directories(properdesktop-display PUBLIC src)
target_link_libraries(properdesktop-display PUBLIC properdesktop-core ProperDesktop::ToolkitBase)
set_target_properties(properdesktop-display PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-display-ipc STATIC
    src/ipc/display_config_service.cpp
    src/ipc/display_config_service.h
    src/ipc/display_dbus_types.cpp
    src/ipc/display_dbus_types.h
)
target_include_directories(properdesktop-display-ipc PUBLIC src)
target_link_libraries(properdesktop-display-ipc PUBLIC properdesktop-display)
set_target_properties(properdesktop-display-ipc PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-wayland STATIC
    src/wayland/client_tracker.cpp
    src/wayland/client_tracker.h
    src/wayland/scoped_wl.h
    src/wayland/resource_reference.h
    src/wayland/serial_tracker.cpp
    src/wayland/serial_tracker.h
    src/wayland/server.cpp
    src/wayland/server.h
    src/wayland/server_util.cpp
    src/wayland/server_util.h
    src/wayland/security_delegate.cpp
    src/wayland/security_delegate.h
)

target_include_directories(properdesktop-wayland PUBLIC src)
target_link_libraries(properdesktop-wayland PUBLIC properdesktop-core PkgConfig::WaylandServer)
set_target_properties(properdesktop-wayland PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-sync STATIC
    src/render/sync_fence.cpp
    src/render/sync_fence.h
    src/render/drm_syncobj.cpp
    src/render/drm_syncobj.h
)
target_include_directories(properdesktop-sync PUBLIC src)
target_link_libraries(properdesktop-sync PUBLIC properdesktop-core PkgConfig::Libdrm)
set_target_properties(properdesktop-sync PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-scene STATIC
    src/scene/window_visual_node.cpp
    src/scene/motion.cpp
    src/scene/motion.h
    src/scene/snap_preview_node.cpp
    src/scene/snap_preview_node.h
    src/scene/screen_transition_node.cpp
    src/scene/screen_transition_node.h
    src/scene/desk_switch_node.cpp
    src/scene/desk_switch_node.h
    src/scene/finger_track.cpp
    src/scene/finger_track.h
    src/scene/overview_node.cpp
    src/scene/overview_node.h
    src/scene/slide_out_node.cpp
    src/scene/slide_out_node.h
    src/scene/window_animation.cpp
    src/scene/window_animation.h
    src/scene/node.cpp
    src/scene/node.h
    src/scene/scene.cpp
    src/scene/scene.h
    src/scene/surface_node.cpp
    src/scene/surface_node.h
    src/scene/wallpaper_node.cpp
    src/scene/wallpaper_node.h
    src/scene/overlay_candidate.cpp
    src/scene/overlay_candidate.h
    src/render/cursor_sink.h
    src/render/scanout.h
    src/render/renderer.cpp
    src/render/renderer.h
    src/render/rounded_shape.cpp
    src/render/rounded_shape.h
    src/render/shape_mask.cpp
    src/render/shape_mask.h
    src/render/output_render_context.h
    src/render/dmabuf.h
    src/render/texture.h
)
target_include_directories(properdesktop-scene PUBLIC src)
target_link_libraries(properdesktop-scene PUBLIC properdesktop-core properdesktop-sync ProperDesktop::ToolkitBase ProperDesktop::Toolkit)
set_target_properties(properdesktop-scene PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-compositor-core STATIC
    src/compositor/buffer.cpp
    src/compositor/buffer.h
    src/compositor/dmabuf_buffer.cpp
    src/compositor/dmabuf_buffer.h
    src/compositor/compositor.cpp
    src/compositor/compositor.h
    src/compositor/shm_pool.cpp
    src/compositor/shm_pool.h
    src/compositor/sub_surface.cpp
    src/compositor/sub_surface.h
    src/wayland/protocols/wp_presentation.cpp
    src/compositor/output.h
    src/compositor/output_scale.h
    src/wayland/protocols/wp_fractional_scale.cpp
    src/wayland/protocols/wp_fractional_scale.h
    src/wayland/protocols/wl_subcompositor.cpp
    src/wayland/protocols/wl_subcompositor.h
    src/wayland/protocols/image_capture.cpp
    src/compositor/capture_source.h
    src/wayland/protocols/wl_output.cpp
    src/wayland/protocols/wl_output.h
    src/wayland/protocols/wp_viewporter.cpp
    src/wayland/protocols/wp_viewporter.h
    src/compositor/surface.cpp
    src/compositor/surface.h
    src/compositor/menu_model.h
    src/wayland/protocols/wl_compositor.cpp
    src/wayland/protocols/wl_compositor.h
    src/wayland/protocols/wl_shm.cpp
    src/wayland/protocols/wl_shm.h
)
target_include_directories(properdesktop-compositor-core PUBLIC src)
target_link_libraries(properdesktop-compositor-core PUBLIC properdesktop-wayland properdesktop-scene properdesktop-protocols
    ProperDesktop::Toolkit)
set_target_properties(properdesktop-compositor-core PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-input-dbus STATIC src/input/dbus_export.cpp src/input/dbus_export.h)
target_include_directories(properdesktop-input-dbus PUBLIC src)
target_link_libraries(properdesktop-input-dbus PUBLIC properdesktop-core ProperDesktop::ToolkitBase)

add_library(properdesktop-mouse-settings STATIC
    src/input/mouse_settings.cpp
    src/input/mouse_settings.h
    src/input/pointer_acceleration.cpp
    src/input/pointer_acceleration.h
    src/input/touchpad_click.cpp
    src/input/touchpad_click.h
    src/input/touchpad_settings.cpp
    src/input/touchpad_settings.h
    src/input/touchscreen_settings.cpp
    src/input/touchscreen_settings.h
)
target_include_directories(properdesktop-mouse-settings PUBLIC src)
target_link_libraries(properdesktop-mouse-settings PUBLIC properdesktop-input-dbus)

add_library(properdesktop-keyboard-settings STATIC
    src/input/keyboard_settings.cpp
    src/input/keyboard_settings.h
)
target_include_directories(properdesktop-keyboard-settings PUBLIC src)
target_link_libraries(properdesktop-keyboard-settings PUBLIC properdesktop-input-dbus)

add_library(properdesktop-input-method-settings STATIC
    src/input/input_method_settings.cpp
    src/input/input_method_settings.h
)
target_include_directories(properdesktop-input-method-settings PUBLIC src)
target_link_libraries(properdesktop-input-method-settings PUBLIC properdesktop-input-dbus)

add_library(properdesktop-input STATIC
    src/clipboard/selection.cpp
    src/clipboard/selection.h
    src/clipboard/stored_source.cpp
    src/clipboard/stored_source.h
    src/input/types.cpp
    src/input/types.h
    src/input/xkb_tracker.cpp
    src/input/xkb_tracker.h
    src/input/key_translator.cpp
    src/input/key_translator.h
    src/input/hit_test.cpp
    src/input/hit_test.h
    src/input/keyboard_delegate.h
    src/input/pointer_delegate.h
    src/input/seat_observer.h
    src/input/drag.cpp
    src/input/drag.h
    src/input/seat.cpp
    src/input/seat.h
    src/input/keyboard.cpp
    src/input/keyboard.h
    src/input/pointer.cpp
    src/input/pointer.h
    src/input/touch.cpp
    src/input/touch.h
    src/input/touch_delegate.h
    src/input/pointer_constraint_delegate.h
    src/input/pointer_constraints.cpp
    src/input/pointer_constraints.h
    src/input/gesture_filter.h
    src/input/pointer_gesture_delegate.h
    src/input/relative_pointer_delegate.h
    src/input/shortcuts_inhibition.cpp
    src/input/shortcuts_inhibition.h
    src/input/event_sink.h
    src/input/input_manager.cpp
    src/input/input_manager.h
)
target_include_directories(properdesktop-input PUBLIC src)
target_link_libraries(properdesktop-input PUBLIC properdesktop-display properdesktop-compositor-core ProperDesktop::ToolkitBase
    PkgConfig::Xkbcommon)
set_target_properties(properdesktop-input PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-wm STATIC
    src/wm/types.cpp
    src/wm/types.h
    src/wm/wm_event.cpp
    src/wm/wm_event.h
    src/wm/anchored_placement.cpp
    src/wm/anchored_placement.h
    src/wm/placement.cpp
    src/wm/placement.h
    src/wm/stacking.cpp
    src/wm/stacking.h
    src/wm/work_area.cpp
    src/wm/work_area.h
    src/wm/window_observer.h
    src/wm/window_visual_controller.cpp
    src/wm/window_visual_controller.h
    src/decoration/window_visual_style.cpp
    src/decoration/window_visual_style.h
    src/wm/window.cpp
    src/wm/window.h
    src/wm/window_shadow.h
    src/wm/shadow_types.cpp
    src/wm/shadow_types.h
    src/wm/shadow_controller_delegate.h
    src/wm/shadow_controller.cpp
    src/wm/shadow_controller.h
    src/wm/wm_shadow_controller_delegate.cpp
    src/wm/wm_shadow_controller_delegate.h
    src/wm/window_state.cpp
    src/wm/window_state.h
    src/wm/activation.cpp
    src/wm/activation.h
    src/wm/transient_manager.cpp
    src/wm/transient_manager.h
    src/wm/window_modality_controller.cpp
    src/wm/window_modality_controller.h
    src/wm/window_positioner.cpp
    src/wm/window_positioner.h
    src/wm/window_snap.cpp
    src/wm/window_snap.h
    src/wm/resize_handles.cpp
    src/wm/resize_handles.h
    src/wm/window_resizer.cpp
    src/wm/window_resizer.h
    src/wm/snap_drag_controller.cpp
    src/wm/snap_drag_controller.h
    src/wm/toplevel_event_handler.cpp
    src/wm/toplevel_event_handler.h
    src/wm/toplevel_drag.cpp
    src/wm/toplevel_drag.h
    src/wm/window_manager.cpp
    src/wm/window_manager.h
    src/wm/quit_all.cpp
    src/wm/quit_all.h
    src/wm/dock_layout_manager.cpp
    src/wm/dock_layout_manager.h
    src/wm/dock_preferences.cpp
    src/wm/dock_preferences.h
    src/wm/shell_surface.cpp
    src/wm/shell_surface.h
    src/wm/show_desktop_controller.cpp
    src/wm/show_desktop_controller.h
    src/wm/status_items.cpp
    src/wm/status_items.h
    src/wm/mru_window_tracker.cpp
    src/wm/mru_window_tracker.h
    src/wm/window_cycle_controller.cpp
    src/wm/window_cycle_controller.h
    src/wm/desks/desk.cpp
    src/wm/desks/desk.h
    src/wm/desks/desk_animation.cpp
    src/wm/desks/desk_animation.h
    src/wm/desks/desks_controller.cpp
    src/wm/desks/desks_controller.h
    src/wm/desks/desk_preferences.cpp
    src/wm/desks/desk_preferences.h
    src/wm/desks/fullscreen_desks.cpp
    src/wm/desks/fullscreen_desks.h
    src/wm/gestures/wm_gesture_handler.cpp
    src/wm/gestures/wm_gesture_handler.h
    src/wm/overview/overview_controller.cpp
    src/wm/overview/overview_controller.h
    src/wm/overview/overview_layout.cpp
    src/wm/overview/overview_layout.h
    src/wm/demo_controller.cpp
    src/wm/demo_controller.h
)
target_include_directories(properdesktop-wm PUBLIC src)
target_link_libraries(properdesktop-wm PUBLIC properdesktop-input properdesktop-compositor-core properdesktop-display
    properdesktop-core ProperDesktop::ToolkitBase)
set_target_properties(properdesktop-wm PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-decoration STATIC
    src/decoration/shadow.cpp
    src/decoration/shadow.h
    src/decoration/shadow_node.cpp
    src/decoration/shadow_node.h
    src/decoration/raster.cpp
    src/decoration/raster.h
    src/decoration/text.cpp
    src/decoration/text.h
    src/decoration/decoration_theme.cpp
    src/decoration/decoration_theme.h
    src/decoration/overview_window_frame.cpp
    src/decoration/overview_window_frame.h
    src/decoration/caption_button.cpp
    src/decoration/caption_button.h
    src/decoration/decoration_node.cpp
    src/decoration/decoration_node.h
    src/decoration/decoration.cpp
    src/decoration/decoration.h
    src/decoration/theme_preferences.cpp
    src/decoration/theme_preferences.h
)
target_include_directories(properdesktop-decoration PUBLIC src)
target_link_libraries(properdesktop-decoration PUBLIC properdesktop-wm properdesktop-scene ProperDesktop::Toolkit
    PRIVATE PkgConfig::TitleText)
set_target_properties(properdesktop-decoration PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-wayland-input STATIC
    src/wayland/protocols/input_method.cpp
    src/wayland/protocols/input_method.h
    src/wayland/protocols/text_input.h
    src/wayland/protocols/text_input_v3.cpp
    src/wayland/protocols/zwp_text_input_v1.cpp
    src/wayland/input_method_launcher.cpp
    src/wayland/input_method_launcher.h
    src/wayland/protocols/idle_notify.cpp
    src/wayland/protocols/zwp_idle_inhibit_manager.cpp
    src/wayland/protocols/zwp_keyboard_shortcuts_inhibit_manager.cpp
    src/wayland/protocols/zwp_keyboard_shortcuts_inhibit_manager.h
    src/wayland/protocols/zwp_pointer_constraints.cpp
    src/wayland/protocols/zwp_pointer_gestures.cpp
    src/wayland/protocols/zwp_pointer_gestures.h
    src/wayland/protocols/zwp_pointer_constraints.h
    src/wayland/protocols/zwp_relative_pointer_manager.cpp
    src/wayland/protocols/zwp_relative_pointer_manager.h
    src/wayland/idle_inhibition.cpp
    src/wayland/idle_inhibition.h
    src/wayland/protocols/cursor_shape.cpp
    src/wayland/protocols/wl_seat.cpp
    src/wayland/protocols/wl_seat.h
    src/wayland/protocols/wayland_keyboard_delegate.cpp
    src/wayland/protocols/wayland_keyboard_delegate.h
    src/wayland/protocols/wayland_pointer_delegate.cpp
    src/wayland/protocols/wayland_pointer_delegate.h
    src/wayland/protocols/wayland_touch_delegate.cpp
    src/wayland/protocols/wayland_touch_delegate.h
    src/wayland/protocols/data_control.cpp
    src/wayland/protocols/data_control.h
    src/wayland/protocols/primary_selection.cpp
    src/wayland/protocols/primary_selection.h
    src/wayland/protocols/wl_data_device_manager.cpp
    src/wayland/protocols/wl_data_device_manager.h
)
target_include_directories(properdesktop-wayland-input PUBLIC src)
target_link_libraries(properdesktop-wayland-input PUBLIC properdesktop-input properdesktop-wayland)
set_target_properties(properdesktop-wayland-input PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-session STATIC
    src/session/applications_service.cpp
    src/session/applications_service.h
    src/session/power_button.cpp
    src/session/power_button.h
    src/session/screen_saver_service.cpp
    src/session/screen_saver_service.h
    src/session/secure_container.cpp
    src/session/secure_container.h
    src/session/session_controller.cpp
    src/session/session_controller.h
    src/session/session_state.h
    src/session/session_teardown.cpp
    src/session/session_teardown.h
    src/session/sleep_lock.cpp
    src/session/sleep_lock.h
    src/session/window_debug_service.cpp
    src/session/window_debug_service.h
)
target_include_directories(properdesktop-session PUBLIC src)
target_link_libraries(properdesktop-session PUBLIC properdesktop-compositor-core properdesktop-wm ProperDesktop::Toolkit)
set_target_properties(properdesktop-session PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-computer-control STATIC
    src/computer_control/controller.cpp
    src/computer_control/controller.h
    src/computer_control/permission_consent.cpp
    src/computer_control/permission_consent.h
    src/computer_control/remembered_apps.cpp
    src/computer_control/remembered_apps.h
)
target_include_directories(properdesktop-computer-control PUBLIC src)
target_link_libraries(properdesktop-computer-control PUBLIC properdesktop-input properdesktop-input-dbus ProperDesktop::ToolkitBase)
set_target_properties(properdesktop-computer-control PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-accelerators STATIC
    src/accelerators/accelerator.cpp
    src/accelerators/accelerator.h
    src/accelerators/accelerator_actions.cpp
    src/accelerators/accelerator_actions.h
    src/accelerators/accelerator_commands.cpp
    src/accelerators/accelerator_commands.h
    src/accelerators/accelerator_configuration.cpp
    src/accelerators/accelerator_configuration.h
    src/accelerators/accelerator_controller.cpp
    src/accelerators/accelerator_controller.h
    src/accelerators/accelerator_delegate.cpp
    src/accelerators/accelerator_delegate.h
    src/accelerators/accelerator_filter.cpp
    src/accelerators/accelerator_filter.h
    src/accelerators/accelerator_history.cpp
    src/accelerators/accelerator_history.h
    src/accelerators/accelerator_launchpad_state_machine.cpp
    src/accelerators/accelerator_launchpad_state_machine.h
    src/accelerators/accelerator_manager.cpp
    src/accelerators/accelerator_manager.h
    src/accelerators/accelerator_map.h
    src/accelerators/accelerator_service.cpp
    src/accelerators/accelerator_service.h
    src/accelerators/accelerator_table.h
    src/accelerators/hot_corner_controller.cpp
    src/accelerators/hot_corner_controller.h
    src/accelerators/hot_corner_settings.cpp
    src/accelerators/hot_corner_settings.h
    src/accelerators/menu_shortcut.cpp
    src/accelerators/menu_shortcut.h
)
target_include_directories(properdesktop-accelerators PUBLIC src)
target_link_libraries(properdesktop-accelerators PUBLIC properdesktop-wm properdesktop-session properdesktop-input-dbus PkgConfig::Xkbcommon)
set_target_properties(properdesktop-accelerators PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-wayland-shell STATIC
    src/wayland/protocols/dmabuf_scanout_policy.cpp
    src/wayland/protocols/dmabuf_scanout_policy.h
    src/wayland/protocols/xdg_activation.cpp
    src/wayland/protocols/xdg_activation.h
    src/wayland/protocols/application_menu.cpp
    src/wayland/protocols/dbus_address.cpp
    src/wayland/protocols/dbus_address.h
    src/wayland/protocols/gtk_shell.cpp
    src/wayland/protocols/gtk_shell.h
    src/wayland/protocols/main_menu.cpp
    src/wayland/protocols/main_menu.h
    src/wayland/protocols/xdg_positioner.cpp
    src/wayland/protocols/xdg_positioner.h
    src/wayland/protocols/xdg_shell.cpp
    src/wayland/protocols/xdg_shell.h
    src/wayland/protocols/status_item.cpp
    src/wayland/protocols/status_item.h
    src/wayland/protocols/window_hints.cpp
    src/wayland/protocols/window_hints.h
    src/wayland/protocols/window_placement.cpp
    src/wayland/protocols/window_placement.h
    src/wayland/protocols/surface_blur.cpp
    src/wayland/protocols/surface_blur.h
    src/wayland/protocols/xdg_decoration.cpp
    src/wayland/protocols/xdg_decoration.h
    src/wayland/protocols/xdg_dialog.cpp
    src/wayland/protocols/xdg_dialog.h
    src/wayland/protocols/xdg_foreign.cpp
    src/wayland/protocols/xdg_foreign.h
    src/wayland/protocols/xdg_toplevel_drag.cpp
    src/wayland/protocols/xdg_toplevel_drag.h
)
target_include_directories(properdesktop-wayland-shell PUBLIC src)
target_link_libraries(properdesktop-wayland-shell
    PUBLIC properdesktop-wm properdesktop-wayland properdesktop-protocols properdesktop-wayland-input
           properdesktop-wayland-dmabuf)
set_target_properties(properdesktop-wayland-shell PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-wayland-private-shell STATIC
    src/wayland/privileged_client.cpp
    src/wayland/privileged_client.h
    src/wayland/shell_control_channel.cpp
    src/wayland/shell_control_channel.h
    src/wayland/protocols/properdesktop_shell.cpp
    src/wayland/protocols/properdesktop_shell.h
    src/wayland/protocols/foreign_toplevel_list.cpp
    src/wayland/protocols/foreign_toplevel_list.h
)
target_include_directories(properdesktop-wayland-private-shell PUBLIC src)
target_link_libraries(properdesktop-wayland-private-shell
    PUBLIC properdesktop-wm properdesktop-wayland properdesktop-protocols)
set_target_properties(properdesktop-wayland-private-shell PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-wayland-dmabuf STATIC
    src/wayland/protocols/dmabuf_feedback_manager.cpp
    src/wayland/protocols/dmabuf_feedback_manager.h
    src/wayland/protocols/linux_dmabuf.cpp
    src/wayland/protocols/linux_dmabuf.h
    src/wayland/protocols/linux_drm_syncobj.cpp
    src/wayland/protocols/linux_drm_syncobj.h
)
target_include_directories(properdesktop-wayland-dmabuf PUBLIC src)
target_link_libraries(properdesktop-wayland-dmabuf
    PUBLIC properdesktop-compositor-core properdesktop-wayland properdesktop-protocols properdesktop-sync
    PRIVATE PkgConfig::Libdrm)
set_property(TARGET properdesktop-wayland-dmabuf PROPERTY POSITION_INDEPENDENT_CODE ON)

set(PROPERDESKTOP_SHADER_NAMES
    color_vertex_shader color_fragment_shader
    texture_vertex_shader texture_fragment_shader
    blur_fragment_shader blur_mask_fragment_shader glass_fragment_shader
    external_fragment_shader external_extension
    rounded_clip clipped_precision clipped_main)
set(PROPERDESKTOP_SHADER_FILES)
foreach(name IN LISTS PROPERDESKTOP_SHADER_NAMES)
    list(APPEND PROPERDESKTOP_SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/render/shaders/${name}.glsl")
endforeach()
set(PROPERDESKTOP_SHADER_HEADER "${CMAKE_CURRENT_BINARY_DIR}/render_shaders.h")
add_custom_command(OUTPUT "${PROPERDESKTOP_SHADER_HEADER}"
    COMMAND ${CMAKE_COMMAND}
        "-DOUTPUT=${PROPERDESKTOP_SHADER_HEADER}"
        "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/src/render/shaders"
        "-DSHADERS=${PROPERDESKTOP_SHADER_NAMES}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/src/render/shaders/embed.cmake"
    DEPENDS ${PROPERDESKTOP_SHADER_FILES} src/render/shaders/embed.cmake
    VERBATIM)
add_library(properdesktop-render STATIC
    "${PROPERDESKTOP_SHADER_HEADER}"
    src/render/egl_renderer.cpp
    src/render/egl_renderer.h
    src/render/egl_context.cpp
    src/render/egl_context.h
    src/render/egl_render_target.cpp
    src/render/egl_render_target.h
    src/render/egl_fence.cpp
    src/render/egl_fence.h
    src/render/gpu_timer.cpp
    src/render/gpu_timer.h
    src/render/egl_dmabuf_importer.cpp
    src/render/egl_dmabuf_importer.h
    src/render/gl_texture.cpp
    src/render/gl_texture.h
    src/render/gl_program.cpp
    src/render/gl_program.h
    src/render/gl_render_pass.cpp
    src/render/gl_render_pass.h
    src/render/backdrop_filter.cpp
    src/render/backdrop_filter.h
    src/render/drop_shadow.cpp
    src/render/drop_shadow.h
    src/render/gaussian_kernel.cpp
    src/render/gaussian_kernel.h
    src/render/color_transform.h
    src/render/texture.h
)
target_include_directories(properdesktop-render PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(properdesktop-render
    PUBLIC properdesktop-scene PkgConfig::Egl PkgConfig::GlesV2
    PRIVATE PkgConfig::Gbm PkgConfig::Libdrm
)
set_target_properties(properdesktop-render PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-backend STATIC
    src/backend/drm/amdgpu_boost.cpp
    src/backend/drm/amdgpu_boost.h
    src/backend/drm/drm_commit_coordinator.cpp
    src/backend/drm/drm_commit_coordinator.h
    src/backend/drm/drm_configuration_planner.cpp
    src/backend/drm/drm_configuration_planner.h
    src/backend/drm/drm_mode_fallback_planner.cpp
    src/backend/drm/drm_mode_fallback_planner.h
    src/backend/drm/drm_multi_device_planner.cpp
    src/backend/drm/drm_multi_device_planner.h
    src/backend/drm/drm_hotplug_monitor.cpp
    src/backend/drm/drm_hotplug_monitor.h
    src/backend/session/session.h
    src/backend/session/libseat_session.cpp
    src/backend/session/libseat_session.h
    src/backend/drm/drm_backend.cpp
    src/backend/drm/drm_backend.h
    src/backend/drm/drm_device_api.h
    src/backend/drm/drm_device.cpp
    src/backend/drm/drm_device.h
    src/backend/drm/drm_plane.cpp
    src/backend/drm/drm_plane.h
    src/backend/drm/plane_assignment.cpp
    src/backend/drm/plane_assignment.h
    src/backend/drm/drm_cursor.cpp
    src/backend/drm/drm_cursor.h
    src/backend/drm/scanout_controller.cpp
    src/backend/drm/scanout_controller.h
    src/backend/drm/drm_framebuffer.cpp
    src/backend/drm/drm_framebuffer.h
    src/backend/drm/drm_output.cpp
    src/backend/drm/drm_output.h
    src/backend/drm/drm_properties.cpp
    src/backend/drm/drm_properties.h
    src/backend/drm/drm_selection.cpp
    src/backend/drm/drm_selection.h
    src/backend/drm/gpu_boost_policy.cpp
    src/backend/drm/gpu_boost_policy.h
    src/backend/input/libinput_backend.cpp
    src/backend/input/libinput_backend.h
    src/backend/input/touchpad_contacts.cpp
    src/backend/input/touchpad_contacts.h
)
target_link_libraries(properdesktop-backend
    PUBLIC properdesktop-core properdesktop-render properdesktop-input properdesktop-mouse-settings PkgConfig::Libdrm
    PRIVATE PkgConfig::Libseat PkgConfig::Gbm PkgConfig::Libinput PkgConfig::Libudev
)
set_target_properties(properdesktop-backend PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-output-scheduler STATIC src/render/output_scheduler.cpp)
target_include_directories(properdesktop-output-scheduler PUBLIC src)
target_link_libraries(properdesktop-backend PUBLIC properdesktop-output-scheduler)

add_library(properdesktop-display-runtime STATIC src/display/display_runtime.cpp)
target_link_libraries(properdesktop-display-runtime PUBLIC properdesktop-backend properdesktop-wayland-shell
    properdesktop-wm properdesktop-display properdesktop-wayland-input properdesktop-cursor)
add_library(properdesktop-toolkit-host STATIC
    src/toolkit/toolkit_host.cpp src/toolkit/toolkit_host.h
    src/toolkit/compositor_colors.cpp src/toolkit/compositor_colors.h)
target_include_directories(properdesktop-toolkit-host PUBLIC src)
target_link_libraries(properdesktop-toolkit-host PUBLIC ProperDesktop::Toolkit properdesktop-render)

find_library(PAM_LIBRARY pam REQUIRED)
add_executable(properdesktop-auth-helper src/auth/auth_helper.cpp)
target_link_libraries(properdesktop-auth-helper PRIVATE ${PAM_LIBRARY})
install(TARGETS properdesktop-auth-helper RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
# Fedora/RHEL use authselect's system-auth rather than Debian's common-*.
set(_properdesktop_pam_default "${CMAKE_CURRENT_SOURCE_DIR}/data/properdesktop-auth")
if(EXISTS "/etc/pam.d/system-auth" AND NOT EXISTS "/etc/pam.d/common-auth")
    set(_properdesktop_pam_default "${CMAKE_CURRENT_SOURCE_DIR}/data/properdesktop-auth-fedora")
endif()
set(PROPERDESKTOP_PAM_CONFIG "${_properdesktop_pam_default}" CACHE FILEPATH
    "PAM service file for Proper Desktop lock-screen authentication")
install(FILES "${PROPERDESKTOP_PAM_CONFIG}"
        DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/pam.d RENAME properdesktop-auth)
# The fingerprint half runs beside properdesktop-auth; it carries pam_fprintd alone.
set(_properdesktop_pam_fingerprint_default
    "${CMAKE_CURRENT_SOURCE_DIR}/data/properdesktop-auth-fingerprint")
if(EXISTS "/etc/pam.d/system-auth" AND NOT EXISTS "/etc/pam.d/common-auth")
    set(_properdesktop_pam_fingerprint_default
        "${CMAKE_CURRENT_SOURCE_DIR}/data/properdesktop-auth-fingerprint-fedora")
endif()
set(PROPERDESKTOP_PAM_FINGERPRINT_CONFIG "${_properdesktop_pam_fingerprint_default}" CACHE FILEPATH
    "PAM service file for Proper Desktop fingerprint unlock")
install(FILES "${PROPERDESKTOP_PAM_FINGERPRINT_CONFIG}"
        DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/pam.d RENAME properdesktop-auth-fingerprint)
add_library(properdesktop-auth STATIC
    src/auth/auth_status.cpp src/auth/auth_status.h
    src/auth/auth_ui.cpp src/auth/auth_ui.h
    src/auth/auth_view.cpp src/auth/auth_view.h
    src/auth/auth_controller.cpp src/auth/auth_controller.h src/auth/auth_backend.h
    src/auth/pam_backend.cpp src/auth/pam_backend.h
    src/auth/greetd_backend.cpp src/auth/greetd_backend.h
    src/auth/login_sessions.cpp src/auth/login_sessions.h)
target_include_directories(properdesktop-auth PUBLIC src)
# The menu bar's items and backdrop rules come from the shell's libraries,
# defined further down.
find_package(ProperDesktopClients REQUIRED COMPONENTS accounts)
target_link_libraries(properdesktop-auth PUBLIC properdesktop-session properdesktop-toolkit-host
    properdesktop-shell-bar-items properdesktop-shell-wallpaper PRIVATE PkgConfig::TitleText ProperDesktop::AccountsClient)
target_compile_definitions(properdesktop-auth PRIVATE
    PROPERDESKTOP_AUTH_HELPER="${CMAKE_INSTALL_FULL_LIBEXECDIR}/properdesktop-auth-helper")

add_library(properdesktop-switcher STATIC
    src/switcher/application_info.cpp src/switcher/application_info.h
    src/switcher/switcher_view.cpp src/switcher/switcher_view.h)
target_include_directories(properdesktop-switcher PUBLIC src)
target_link_libraries(properdesktop-switcher PUBLIC properdesktop-wm properdesktop-decoration properdesktop-scene
    properdesktop-toolkit-host)

add_library(properdesktop-shell STATIC
    src/shell/shell.cpp src/shell/shell.h
    src/shell/shell_data.cpp src/shell/shell_data.h
    src/shell/shell_drag.cpp src/shell/shell_drag.h
    src/shell/shell_host.cpp src/shell/shell_host.h
    src/shell/shell_input.cpp src/shell/shell_input.h
    src/shell/shell_text_input.cpp src/shell/shell_text_input.h
    src/shell/shell_window.cpp src/shell/shell_window.h
    src/shell/window_renderer.cpp src/shell/window_renderer.h)
target_include_directories(properdesktop-shell PUBLIC src)
target_link_libraries(properdesktop-shell PUBLIC properdesktop-wm properdesktop-input properdesktop-display
    properdesktop-decoration properdesktop-toolkit-host properdesktop-shell-common PRIVATE properdesktop-wayland-shell)
target_compile_definitions(properdesktop-shell PRIVATE
    PROPERDESKTOP_SHELL_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/properdesktop-compositor/shell")
install(DIRECTORY data/shell/ DESTINATION ${CMAKE_INSTALL_DATADIR}/properdesktop-compositor/shell)

add_library(properdesktop-headless STATIC
    src/backend/headless/headless_backend.cpp
    src/backend/headless/headless_backend.h
)
target_include_directories(properdesktop-headless PUBLIC src)
target_link_libraries(properdesktop-headless PUBLIC properdesktop-compositor-core)
set_target_properties(properdesktop-headless PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(properdesktop-wayland-globals STATIC
    src/wayland/protocol_globals.cpp
    src/wayland/protocol_globals.h
    src/wayland/protocols/properdesktop_computer_control.cpp
    src/wayland/protocols/properdesktop_computer_control.h
)
target_include_directories(properdesktop-wayland-globals PUBLIC src)
target_link_libraries(properdesktop-wayland-globals
    PUBLIC properdesktop-wayland properdesktop-compositor-core properdesktop-wayland-input
           properdesktop-wayland-shell properdesktop-wayland-private-shell properdesktop-wm
           properdesktop-input properdesktop-input-method-settings properdesktop-computer-control
)
set_target_properties(properdesktop-wayland-globals PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(properdesktop-compositor
    src/main.cpp
    src/core/compositor_options.cpp
    src/core/compositor_options.h
    src/core/computer_control_environment.cpp
    src/core/computer_control_environment.h
    src/core/compositor_shell.cpp
    src/core/compositor_shell.h
)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-display-runtime
    properdesktop-auth properdesktop-switcher properdesktop-shell properdesktop-wayland-globals properdesktop-headless)
target_compile_definitions(properdesktop-compositor PRIVATE
    PROPERDESKTOP_PORTAL_BACKEND="${CMAKE_INSTALL_FULL_LIBEXECDIR}/xdg-desktop-portal-properdesktop"
    PROPERDESKTOP_SCREEN_RECORDER="${CMAKE_INSTALL_FULL_LIBEXECDIR}/properdesktop-screenrecorder")
target_link_libraries(properdesktop-compositor
    PRIVATE properdesktop-wayland properdesktop-compositor-core properdesktop-backend properdesktop-scene
            properdesktop-input properdesktop-wayland-input properdesktop-wm
            properdesktop-wayland-shell properdesktop-decoration properdesktop-wayland-dmabuf
            properdesktop-wayland-private-shell properdesktop-display-ipc properdesktop-input-method-settings
            properdesktop-keyboard-settings properdesktop-accelerators)

pkg_check_modules(WaylandCaptureClient REQUIRED IMPORTED_TARGET wayland-client)
add_executable(properdesktop-screenshot src/tools/screenshot.cpp)
target_link_libraries(properdesktop-screenshot PRIVATE PkgConfig::WaylandCaptureClient ProperDesktop::ToolkitBase)

add_executable(properdesktop-displayctl src/tools/displayctl.cpp)
target_link_libraries(properdesktop-displayctl PRIVATE properdesktop-display-ipc ProperDesktop::ToolkitBase)

option(PROPERDESKTOP_ENABLE_XWAYLAND "Build the native Xwayland frontend" ON)
if(PROPERDESKTOP_ENABLE_XWAYLAND)
    find_package(Threads REQUIRED)
    pkg_check_modules(Xcb REQUIRED IMPORTED_TARGET xcb xcb-composite xcb-xfixes xcb-shape xcb-sync)
    properdesktop_wayland_protocol(xwayland-shell-v1
        ${WAYLAND_PROTOCOLS_DATADIR}/staging/xwayland-shell/xwayland-shell-v1.xml)
    properdesktop_wayland_protocol(xwayland-keyboard-grab-unstable-v1
        ${WAYLAND_PROTOCOLS_DATADIR}/unstable/xwayland-keyboard-grab/xwayland-keyboard-grab-unstable-v1.xml)
    add_library(properdesktop-xwayland STATIC
        src/xwayland/surface_association.cpp
        src/xwayland/xwayland_shell.cpp
        src/xwayland/xwayland_keyboard_grab.cpp
        src/xwayland/xwayland_server.cpp
        src/xwayland/xwm.cpp
        src/xwayland/selection_bridge.cpp
        src/xwayland/dnd_bridge.cpp
        ${xwayland-shell-v1_SOURCES}
        ${xwayland-keyboard-grab-unstable-v1_SOURCES})
    target_include_directories(properdesktop-xwayland PUBLIC src ${PROPERDESKTOP_PROTOCOL_DIR})
    target_link_libraries(properdesktop-xwayland PUBLIC properdesktop-wayland-private-shell properdesktop-cursor PkgConfig::Xcb
        Threads::Threads ProperDesktop::ToolkitBase)
    target_link_libraries(properdesktop-compositor PRIVATE properdesktop-xwayland)
    target_compile_definitions(properdesktop-compositor PRIVATE PROPERDESKTOP_ENABLE_XWAYLAND)
    install(FILES ${WAYLAND_PROTOCOLS_DATADIR}/staging/xwayland-shell/xwayland-shell-v1.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/properdesktop-compositor)
endif()

install(TARGETS properdesktop-compositor properdesktop-displayctl properdesktop-screenshot RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/input-method-unstable-v2.xml
    ${PROPERDESKTOP_PRIVATE_PROTOCOL_DIR}/virtual-keyboard-unstable-v1.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/properdesktop-compositor)
install(FILES LICENSE LICENSE.chromium LICENSE.sommelier DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/properdesktop-compositor)
install(FILES ${DISPLAY_CONFIG_XML} DESTINATION ${CMAKE_INSTALL_DATADIR}/dbus-1/interfaces)
properdesktop_add_translations(properdesktop-compositor SOURCES src/shell src/auth src/wm/overview
    DESKTOP_FILES data/properdesktop-screenshot.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/properdesktop-screenshot.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

target_link_libraries(properdesktop-wayland-input PUBLIC properdesktop-protocols)

add_library(properdesktop-cursor STATIC src/cursor/theme.cpp src/cursor/cursor.cpp
    src/cursor/output_cursor_router.cpp src/cursor/output_cursor_router.h)
target_include_directories(properdesktop-cursor PUBLIC src)
target_sources(properdesktop-cursor PRIVATE src/cursor/preferences.cpp src/cursor/preferences.h)
target_link_libraries(properdesktop-cursor PUBLIC properdesktop-input properdesktop-display ProperDesktop::Toolkit)
target_link_libraries(properdesktop-compositor PRIVATE properdesktop-cursor)

include(src/shell/backends.cmake)
include(src/shell/components.cmake)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
