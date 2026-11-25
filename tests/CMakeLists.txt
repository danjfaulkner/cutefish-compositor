find_package(GTest)
if(NOT GTest_FOUND)
    message(FATAL_ERROR "BUILD_TESTING=ON needs GoogleTest (Debian: libgtest-dev)")
endif()
pkg_check_modules(WaylandClient REQUIRED IMPORTED_TARGET wayland-client)

add_library(properdesktop-test-loop STATIC loop_test_util.cpp)
target_link_libraries(properdesktop-test-loop PUBLIC properdesktop-core)

# properdesktop_add_test(name libraries...)
function(properdesktop_add_test name)
    add_executable(${name} ${name}.cpp gtest_toolkit_main.cpp)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE GTest::gtest properdesktop-core properdesktop-test-loop
        ${ARGN})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

properdesktop_add_test(main_loop_test properdesktop-toolkit-host)
properdesktop_add_test(core_test)
target_sources(core_test PRIVATE ${PROJECT_SOURCE_DIR}/src/core/compositor_options.cpp)
target_compile_definitions(core_test PRIVATE
    PROPERDESKTOP_PORTAL_BACKEND="${CMAKE_INSTALL_FULL_LIBEXECDIR}/xdg-desktop-portal-properdesktop"
    PROPERDESKTOP_SCREEN_RECORDER="${CMAKE_INSTALL_FULL_LIBEXECDIR}/properdesktop-screenrecorder"
    PROPERDESKTOP_BINDIR="${CMAKE_INSTALL_FULL_BINDIR}")

properdesktop_add_test(image_capture_test properdesktop-compositor-core properdesktop-session PkgConfig::WaylandClient)
properdesktop_add_test(serial_tracker_test properdesktop-wayland)
properdesktop_add_test(client_tracker_test properdesktop-wayland)
properdesktop_add_test(wayland_server_test properdesktop-wayland PkgConfig::WaylandClient)
properdesktop_add_test(scene_test properdesktop-scene)
properdesktop_add_test(shm_pool_test properdesktop-compositor-core)
properdesktop_add_test(buffer_test properdesktop-compositor-core)
properdesktop_add_test(surface_test properdesktop-compositor-core)
properdesktop_add_test(wayland_protocol_test properdesktop-compositor-core properdesktop-wayland-dmabuf
    PkgConfig::WaylandClient)
properdesktop_add_test(xkb_tracker_test properdesktop-input)
properdesktop_add_test(key_translator_test properdesktop-input)
properdesktop_add_test(input_test properdesktop-input)
properdesktop_add_test(computer_control_test properdesktop-computer-control)
properdesktop_add_test(remembered_apps_test properdesktop-computer-control)
properdesktop_add_test(touch_test properdesktop-input)
foreach(test pointer_acceleration_test touchpad_click_test)
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} PRIVATE properdesktop-mouse-settings GTest::gtest_main)
    add_test(NAME ${test} COMMAND ${test})
endforeach()
add_executable(mouse_settings_test mouse_settings_test.cpp)
target_include_directories(mouse_settings_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mouse_settings_test PRIVATE properdesktop-mouse-settings GTest::gtest_main)
add_test(NAME mouse_settings_test COMMAND mouse_settings_test)
add_executable(touchpad_settings_test touchpad_settings_test.cpp)
target_include_directories(touchpad_settings_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(touchpad_settings_test PRIVATE properdesktop-mouse-settings GTest::gtest_main)
add_test(NAME touchpad_settings_test COMMAND touchpad_settings_test)
add_executable(touchscreen_settings_test touchscreen_settings_test.cpp)
target_include_directories(touchscreen_settings_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(touchscreen_settings_test PRIVATE properdesktop-mouse-settings GTest::gtest_main)
add_test(NAME touchscreen_settings_test COMMAND touchscreen_settings_test)
add_executable(keyboard_settings_test keyboard_settings_test.cpp)
target_include_directories(keyboard_settings_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(keyboard_settings_test PRIVATE properdesktop-keyboard-settings GTest::gtest_main)
add_test(NAME keyboard_settings_test COMMAND keyboard_settings_test)
properdesktop_add_test(drm_selection_test properdesktop-backend)
properdesktop_add_test(drm_properties_test properdesktop-backend)
properdesktop_add_test(drm_configuration_planner_test properdesktop-fake-drm)
properdesktop_add_test(drm_multi_device_planner_test properdesktop-backend)
properdesktop_add_test(drm_hotplug_monitor_test properdesktop-backend)
properdesktop_add_test(gpu_boost_policy_test properdesktop-backend)
properdesktop_add_test(libseat_session_test properdesktop-backend)

add_executable(properdesktop-shm-demo manual/shm_client.cpp)
target_link_libraries(properdesktop-shm-demo PRIVATE PkgConfig::WaylandClient)

properdesktop_add_test(sub_surface_test properdesktop-compositor-core)
properdesktop_add_test(wm_test properdesktop-wm)
properdesktop_add_test(subsurface_output_protocol_test properdesktop-compositor-core PkgConfig::WaylandClient)
properdesktop_add_test(seat_protocol_test properdesktop-wayland-input properdesktop-wm
    PkgConfig::WaylandClient PkgConfig::Xkbcommon)
properdesktop_add_test(xdg_positioner_test properdesktop-wayland-shell)
properdesktop_add_test(shell_surface_test properdesktop-decoration)
properdesktop_add_test(decoration_test properdesktop-decoration)
properdesktop_add_test(text_test properdesktop-decoration PkgConfig::TitleText)
properdesktop_add_test(xdg_shell_protocol_test properdesktop-wayland-shell properdesktop-wayland-input
    properdesktop-wayland-private-shell PkgConfig::WaylandClient)
properdesktop_add_test(status_item_protocol_test properdesktop-wayland-shell properdesktop-wayland-input
    PkgConfig::WaylandClient)
properdesktop_add_test(main_menu_protocol_test properdesktop-wayland-shell properdesktop-wayland-input
    PkgConfig::WaylandClient)
properdesktop_add_test(gtk_shell_protocol_test properdesktop-wayland-shell PkgConfig::WaylandClient)
properdesktop_add_test(protocol_binding_test properdesktop-wayland-globals PkgConfig::WaylandClient)
set_tests_properties(protocol_binding_test PROPERTIES TIMEOUT 60)
# PROPERDESKTOP_LIBFUZZER needs clang.
option(PROPERDESKTOP_LIBFUZZER "Build protocol_fuzzer for libFuzzer" OFF)
add_executable(protocol_fuzzer protocol_fuzzer.cpp)
target_include_directories(protocol_fuzzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(protocol_fuzzer PRIVATE properdesktop-wayland-globals PkgConfig::WaylandClient)
if(PROPERDESKTOP_LIBFUZZER)
    target_compile_definitions(protocol_fuzzer PRIVATE PROPERDESKTOP_LIBFUZZER)
    target_compile_options(protocol_fuzzer PRIVATE -fsanitize=fuzzer,address)
    target_link_options(protocol_fuzzer PRIVATE -fsanitize=fuzzer,address)
else()
    add_test(NAME protocol_fuzzer COMMAND protocol_fuzzer)
    set_tests_properties(protocol_fuzzer PROPERTIES TIMEOUT 300)
endif()
properdesktop_add_test(xdg_foreign_protocol_test properdesktop-wayland-shell properdesktop-wayland-input
    PkgConfig::WaylandClient)
properdesktop_add_test(xdg_toplevel_drag_protocol_test properdesktop-wayland-shell properdesktop-wayland-input
    PkgConfig::WaylandClient)
properdesktop_add_test(foreign_toplevel_list_test properdesktop-wayland-private-shell properdesktop-input
    properdesktop-compositor-core PkgConfig::WaylandClient)
properdesktop_add_test(private_shell_protocol_test properdesktop-wayland-private-shell
    properdesktop-wayland-shell properdesktop-wayland-input PkgConfig::WaylandClient)
properdesktop_add_test(work_area_test properdesktop-wm)
properdesktop_add_test(dock_layout_test properdesktop-wm)
properdesktop_add_test(session_lock_test properdesktop-session properdesktop-wm properdesktop-scene properdesktop-decoration)
properdesktop_add_test(accelerator_test properdesktop-accelerators properdesktop-session properdesktop-wm
    properdesktop-scene)
properdesktop_add_test(xdg_decoration_protocol_test properdesktop-wayland-shell properdesktop-wayland-input
    properdesktop-decoration PkgConfig::WaylandClient)
add_executable(properdesktop-subsurface-demo manual/subsurface_client.cpp)
target_link_libraries(properdesktop-subsurface-demo PRIVATE PkgConfig::WaylandClient)

add_executable(properdesktop-xdg-demo manual/xdg_client.cpp)
target_link_libraries(properdesktop-xdg-demo PRIVATE PkgConfig::WaylandClient properdesktop-protocols)

add_executable(properdesktop-decoration-demo manual/decoration_client.cpp)
target_link_libraries(properdesktop-decoration-demo
    PRIVATE PkgConfig::WaylandClient properdesktop-protocols)

add_executable(properdesktop-shell-session-launcher manual/shell_session_launcher.cpp)

add_executable(properdesktop-input-demo manual/input_client.cpp)
target_link_libraries(properdesktop-input-demo PRIVATE PkgConfig::WaylandClient PkgConfig::Xkbcommon)
target_compile_definitions(subsurface_output_protocol_test PRIVATE
    SUBSURFACE_DEMO_PATH="$<TARGET_FILE:properdesktop-subsurface-demo>")
add_dependencies(subsurface_output_protocol_test properdesktop-subsurface-demo)

properdesktop_add_test(output_scheduler_test properdesktop-output-scheduler properdesktop-compositor-core properdesktop-decoration)

properdesktop_add_test(clipboard_protocol_test properdesktop-wayland-input PkgConfig::WaylandClient)
set_tests_properties(clipboard_protocol_test PROPERTIES TIMEOUT 30)

properdesktop_add_test(selection_test properdesktop-input)

properdesktop_add_test(dmabuf_test properdesktop-wayland-dmabuf properdesktop-compositor-core
    PkgConfig::WaylandClient PkgConfig::Libdrm)

properdesktop_add_test(shadow_test properdesktop-decoration)
properdesktop_add_test(shadow_controller_test properdesktop-decoration)
properdesktop_add_test(shadow_pixel_test properdesktop-decoration properdesktop-render)
set_tests_properties(shadow_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")

properdesktop_add_test(motion_test properdesktop-scene)
properdesktop_add_test(window_animation_test properdesktop-scene)
properdesktop_add_test(screen_transition_node_test properdesktop-scene)
properdesktop_add_test(desk_switch_node_test properdesktop-scene)
properdesktop_add_test(desks_test properdesktop-wm)
properdesktop_add_test(overview_layout_test properdesktop-wm)
properdesktop_add_test(overview_node_test properdesktop-scene)
properdesktop_add_test(overview_test properdesktop-wm)
properdesktop_add_test(show_desktop_test properdesktop-wm)
properdesktop_add_test(hot_corner_test properdesktop-accelerators)
properdesktop_add_test(menu_shortcut_test properdesktop-accelerators)

properdesktop_add_test(cursor_test properdesktop-cursor)
properdesktop_add_test(output_cursor_router_test properdesktop-cursor)

properdesktop_add_test(overlay_candidate_test properdesktop-scene)
properdesktop_add_test(plane_assignment_test properdesktop-backend PkgConfig::Libdrm)
properdesktop_add_test(sync_fence_test properdesktop-sync properdesktop-compositor-core)
properdesktop_add_test(commit_readiness_test properdesktop-sync properdesktop-compositor-core)

add_library(properdesktop-fake-drm STATIC fake_drm_device.cpp fake_drm_device.h)
target_include_directories(properdesktop-fake-drm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(properdesktop-fake-drm PUBLIC properdesktop-backend PkgConfig::Libdrm)

properdesktop_add_test(scanout_test properdesktop-fake-drm properdesktop-compositor-core)
properdesktop_add_test(hardware_cursor_test properdesktop-fake-drm properdesktop-cursor properdesktop-session)

properdesktop_add_test(window_hints_protocol_test properdesktop-wayland-shell PkgConfig::WaylandClient)
properdesktop_add_test(surface_blur_protocol_test properdesktop-wayland-shell PkgConfig::WaylandClient)

properdesktop_add_test(dnd_protocol_test properdesktop-cursor properdesktop-wayland-input
    properdesktop-wayland-private-shell PkgConfig::WaylandClient)

properdesktop_add_test(data_control_protocol_test properdesktop-wayland-input properdesktop-wayland-private-shell
    properdesktop-wayland-shell PkgConfig::WaylandClient)

properdesktop_add_test(dnd_toolkit_test properdesktop-wayland-input properdesktop-wayland-shell PkgConfig::WaylandClient)
target_compile_definitions(dnd_toolkit_test PRIVATE GTK_DND_CLIENT="${CMAKE_CURRENT_SOURCE_DIR}/dnd/gtk_client.py")
set_tests_properties(dnd_toolkit_test PROPERTIES TIMEOUT 70)

set_tests_properties(private_shell_protocol_test PROPERTIES TIMEOUT 60)

properdesktop_add_test(viewporter_test properdesktop-compositor-core)
properdesktop_add_test(viewporter_protocol_test properdesktop-compositor-core PkgConfig::WaylandClient)
set_tests_properties(viewporter_protocol_test PROPERTIES TIMEOUT 30)
properdesktop_add_test(viewporter_pixel_test properdesktop-compositor-core properdesktop-input properdesktop-render
    PkgConfig::Gbm PkgConfig::Libdrm)
set_tests_properties(viewporter_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")

properdesktop_add_test(fractional_scale_protocol_test properdesktop-compositor-core PkgConfig::WaylandClient)
set_tests_properties(fractional_scale_protocol_test PROPERTIES TIMEOUT 30)

properdesktop_add_test(fractional_scale_pixel_test properdesktop-compositor-core properdesktop-render)
set_tests_properties(fractional_scale_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")

properdesktop_add_test(output_scale_test properdesktop-input)
properdesktop_add_test(display_types_test properdesktop-display)
properdesktop_add_test(edid_test properdesktop-display)
properdesktop_add_test(display_preferences_test properdesktop-display)
properdesktop_add_test(recommended_scale_test properdesktop-display)
properdesktop_add_test(display_transform_test properdesktop-display)
properdesktop_add_test(mirror_transform_test properdesktop-display)
properdesktop_add_test(display_layout_test properdesktop-display)
properdesktop_add_test(chromeos_display_layout_test properdesktop-display)
properdesktop_add_test(display_configuration_validator_test properdesktop-display)
properdesktop_add_test(display_manager_test properdesktop-display)
properdesktop_add_test(native_display_delegate_test properdesktop-display)
target_sources(native_display_delegate_test PRIVATE
    fake_native_display_delegate.cpp fake_native_display_delegate.h)
properdesktop_add_test(display_configuration_controller_test properdesktop-display)
target_sources(display_configuration_controller_test PRIVATE
    fake_native_display_delegate.cpp fake_native_display_delegate.h)
properdesktop_add_test(night_light_test properdesktop-display)
properdesktop_add_test(display_config_service_test properdesktop-display-ipc)
target_sources(display_config_service_test PRIVATE
    fake_native_display_delegate.cpp fake_native_display_delegate.h)

properdesktop_add_test(presentation_test properdesktop-compositor-core)
properdesktop_add_test(presentation_protocol_test properdesktop-compositor-core PkgConfig::WaylandClient)
add_executable(properdesktop-presentation-client manual/presentation_client.cpp)
target_link_libraries(properdesktop-presentation-client PRIVATE properdesktop-protocols PkgConfig::WaylandClient)

properdesktop_add_test(drm_output_test properdesktop-backend properdesktop-compositor-core PkgConfig::WaylandClient)
target_sources(drm_output_test PRIVATE fake_drm_device.cpp)
set_tests_properties(drm_output_test PROPERTIES TIMEOUT 60 ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")

properdesktop_add_test(window_visual_test properdesktop-decoration)
properdesktop_add_test(window_visual_pixel_test properdesktop-decoration properdesktop-render)
set_tests_properties(window_visual_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
add_executable(toolkit-resize-client visuals/toolkit_client.cpp)
target_link_libraries(toolkit-resize-client PRIVATE ProperDesktop::Toolkit)
# The GTK clients activate portal and gvfs services on the session bus.
properdesktop_add_test(window_visual_toolkit_test properdesktop-wayland-input properdesktop-wayland-shell
    properdesktop-decoration properdesktop-render PkgConfig::WaylandClient)
target_compile_definitions(window_visual_toolkit_test PRIVATE
    TOOLKIT_RESIZE_CLIENT="$<TARGET_FILE:toolkit-resize-client>"
    ELECTRON_VISUAL_CLIENT="${CMAKE_CURRENT_SOURCE_DIR}/visuals/electron_client.js")
add_dependencies(window_visual_toolkit_test toolkit-resize-client)
set_tests_properties(window_visual_toolkit_test PROPERTIES TIMEOUT 70
    ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")

if(PROPERDESKTOP_ENABLE_XWAYLAND)
    properdesktop_add_test(xwayland_test properdesktop-xwayland properdesktop-wayland-shell properdesktop-wayland-input properdesktop-render properdesktop-decoration PkgConfig::WaylandClient)
    add_executable(xwayland-startup-fault manual/xwayland_startup_fault.cpp)
    target_compile_definitions(xwayland_test PRIVATE XWAYLAND_FAULT_PATH="$<TARGET_FILE:xwayland-startup-fault>")
    add_executable(x11-test-client xwayland/x11_client.cpp)
    target_link_libraries(x11-test-client PRIVATE PkgConfig::Xcb)
    target_compile_definitions(xwayland_test PRIVATE X11_TEST_CLIENT="$<TARGET_FILE:x11-test-client>"
        GTK_DND_CLIENT="${CMAKE_CURRENT_SOURCE_DIR}/dnd/gtk_client.py")
    add_dependencies(xwayland_test xwayland-startup-fault x11-test-client)
    set_tests_properties(xwayland_test PROPERTIES TIMEOUT 90 ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
endif()

properdesktop_add_test(display_runtime_test properdesktop-display-runtime properdesktop-session)

properdesktop_add_test(capture_pixel_test properdesktop-render)
properdesktop_add_test(render_internals_test properdesktop-render PkgConfig::Gbm PkgConfig::Libdrm)
set_tests_properties(render_internals_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
set_tests_properties(capture_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
properdesktop_add_test(desk_switch_pixel_test properdesktop-scene properdesktop-render)
set_tests_properties(desk_switch_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
properdesktop_add_test(overview_pixel_test properdesktop-scene properdesktop-render)
set_tests_properties(overview_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
properdesktop_add_test(wallpaper_pixel_test properdesktop-scene properdesktop-render)
set_tests_properties(wallpaper_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
set_tests_properties(image_capture_test PROPERTIES TIMEOUT 15)


properdesktop_add_test(damage_test properdesktop-compositor-core)
properdesktop_add_test(damage_pixel_test properdesktop-scene properdesktop-render)
set_tests_properties(damage_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
properdesktop_add_test(damage_gbm_test properdesktop-scene properdesktop-render PkgConfig::Gbm)
properdesktop_add_test(dmabuf_format_gbm_test properdesktop-render PkgConfig::Gbm)
properdesktop_add_test(capture_dmabuf_gbm_test properdesktop-render PkgConfig::Gbm PkgConfig::Libdrm)

properdesktop_add_test(idle_notify_protocol_test properdesktop-wayland-input PkgConfig::WaylandClient)

properdesktop_add_test(auth_controller_test properdesktop-auth)
properdesktop_add_test(switcher_view_test properdesktop-switcher)
properdesktop_add_test(shell_test properdesktop-shell properdesktop-computer-control properdesktop-session
    properdesktop-wayland-input)
target_sources(shell_test PRIVATE ${PROJECT_SOURCE_DIR}/src/core/computer_control_environment.cpp)
properdesktop_add_test(shell_gpu_pixel_test properdesktop-shell properdesktop-render)
set_tests_properties(shell_gpu_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
add_executable(properdesktop-shell-memory-bench manual/shell_memory_bench.cpp)
target_include_directories(properdesktop-shell-memory-bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(properdesktop-shell-memory-bench PRIVATE properdesktop-shell-osd properdesktop-shell-dock-view
    properdesktop-shell-wallpaper properdesktop-shell-launchpad-view properdesktop-shell-notification-view properdesktop-shell-notification
    properdesktop-shell-screenshot
    properdesktop-decoration properdesktop-render properdesktop-test-loop)
properdesktop_add_test(panel_gpu_pixel_test properdesktop-auth properdesktop-render)
target_include_directories(panel_gpu_pixel_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(panel_gpu_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
properdesktop_add_test(switcher_gpu_pixel_test properdesktop-switcher properdesktop-render)
set_tests_properties(switcher_gpu_pixel_test PROPERTIES ENVIRONMENT "LIBGL_ALWAYS_SOFTWARE=1;EGL_PLATFORM=surfaceless")
properdesktop_add_test(auth_ui_test properdesktop-auth)
target_compile_definitions(auth_ui_test PRIVATE PROPERDESKTOP_PO_DIR="${PROJECT_SOURCE_DIR}/po")
properdesktop_add_test(greetd_backend_test properdesktop-auth)
properdesktop_add_test(screen_saver_service_test properdesktop-session)
properdesktop_add_test(applications_service_test properdesktop-session)

properdesktop_add_test(window_debug_service_test properdesktop-session)

properdesktop_add_test(sleep_lock_test properdesktop-session)

properdesktop_add_test(power_button_test properdesktop-session)

properdesktop_add_test(input_method_protocol_test properdesktop-wayland-input PkgConfig::WaylandClient)
set_tests_properties(input_method_protocol_test PROPERTIES TIMEOUT 45)

add_executable(input_method_settings_test input_method_settings_test.cpp)
target_include_directories(input_method_settings_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(input_method_settings_test PRIVATE properdesktop-input-method-settings
    GTest::gtest_main)
add_test(NAME input_method_settings_test COMMAND input_method_settings_test)

add_executable(input_method_launched_client input_method/launched_client.cpp)
target_link_libraries(input_method_launched_client PRIVATE PkgConfig::WaylandClient)
properdesktop_add_test(input_method_launcher_test properdesktop-wayland-input
    properdesktop-wayland-private-shell PkgConfig::WaylandClient)
target_compile_definitions(input_method_launcher_test PRIVATE
    PROPERDESKTOP_LAUNCHED_CLIENT="$<TARGET_FILE:input_method_launched_client>")
add_dependencies(input_method_launcher_test input_method_launched_client)
set_tests_properties(input_method_launcher_test PROPERTIES TIMEOUT 60)

# Sanitizer builds need a scale of about 5.
set(PROPERDESKTOP_TEST_TIMEOUT_SCALE 1 CACHE STRING "Multiplier for every test's TIMEOUT")
if(NOT PROPERDESKTOP_TEST_TIMEOUT_SCALE EQUAL 1)
    get_directory_property(properdesktop_all_tests TESTS)
    foreach(test ${properdesktop_all_tests})
        get_test_property(${test} TIMEOUT timeout)
        if(timeout AND NOT timeout STREQUAL "NOTFOUND")
            math(EXPR scaled "${timeout} * ${PROPERDESKTOP_TEST_TIMEOUT_SCALE}")
            set_tests_properties(${test} PROPERTIES TIMEOUT ${scaled})
        endif()
    endforeach()
endif()

add_subdirectory(shell)

# The last test fails when a test left a process behind or ran outside the
# launcher; ctest runs it after every other test, also when only some are
# chosen.
add_test(NAME isolation-start COMMAND ${PROPERDESKTOP_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/isolate.py
    --start ${PROPERDESKTOP_TEST_RECORD})
add_test(NAME isolation-check
    COMMAND ${PROPERDESKTOP_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/isolate.py --check ${PROPERDESKTOP_TEST_RECORD}/writes.log
        --since ${PROPERDESKTOP_TEST_RECORD}/since --ctest ${CMAKE_CTEST_COMMAND} ${CMAKE_BINARY_DIR})
set_tests_properties(isolation-start PROPERTIES FIXTURES_SETUP isolation)
set_tests_properties(isolation-check PROPERTIES FIXTURES_CLEANUP isolation)
foreach(directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/shell)
    get_property(tests DIRECTORY ${directory} PROPERTY TESTS)
    list(REMOVE_ITEM tests isolation-start isolation-check)
    set_property(TEST ${tests} DIRECTORY ${directory} APPEND PROPERTY FIXTURES_REQUIRED isolation)
endforeach()
