cmake_minimum_required(VERSION 3.16)
project(SolarFramework VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ImGui Core & Backends
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui")

set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
)

# Solar Framework Library - 14 Specialized Subsystems
set(SOLAR_SOURCES
    "src/anim/animation_manager.cpp"
    "src/anim/easing.cpp"
    "src/anim/smooth_damp.cpp"
    "src/anim/spring.cpp"
    "src/anim/timeline.cpp"
    "src/anim/transitions.cpp"
    "src/audio/audio_engine.cpp"
    "src/audio/sound_effects.cpp"
    "src/audio/wave_synth.cpp"
    "src/auth/auth_manager.cpp"
    "src/auth/hwid_generator.cpp"
    "src/auth/license_screen.cpp"
    "src/config/config_manager.cpp"
    "src/config/json_serializer.cpp"
    "src/config/preset_storage.cpp"
    "src/core/color.cpp"
    "src/core/crypto.cpp"
    "src/core/logger.cpp"
    "src/core/math.cpp"
    "src/core/string_utils.cpp"
    "src/core/timer.cpp"
    "src/fx/glow_fx.cpp"
    "src/fx/particle_system.cpp"
    "src/fx/rotating_border.cpp"
    "src/game/chams_preview.cpp"
    "src/game/esp_preview.cpp"
    "src/game/hitbox_picker.cpp"
    "src/game/keybind_list.cpp"
    "src/game/obb_renderer.cpp"
    "src/game/radar.cpp"
    "src/game/radar_window.cpp"
    "src/game/recoil_visualizer.cpp"
    "src/game/spectator_list.cpp"
    "src/game/trajectory_renderer.cpp"
    "src/game/visuals_renderer.cpp"
    "src/game/watermark.cpp"
    "src/game/screen_watermark.cpp"
    "src/game/fov_renderer.cpp"
    "src/game/skin_changer.cpp"
    "src/hook/hook_bridge.cpp"
    "src/hook/state_guard.cpp"
    "src/icons.cpp"
    "src/icons/vector_icons.cpp"
    "src/notifications/notification_system.cpp"
    "src/notifications/toast.cpp"
    "src/render/drawlist_utils.cpp"
    "src/render/dx11_renderer.cpp"
    "src/render/dx12_renderer.cpp"
    "src/render/font_manager.cpp"
    "src/render/glow_engine.cpp"
    "src/render/imgui_ext.cpp"
    "src/render/shadow_caster.cpp"
    "src/security/hwid_spoofer.cpp"
    "src/security/memory_tools.cpp"
    "src/security/pattern_scanner.cpp"
    "src/security/trace_cleaner.cpp"
    "src/solar.cpp"
    "src/theme/palette.cpp"
    "src/theme/presets.cpp"
    "src/theme/theme_manager.cpp"
    "src/tools/inspector.cpp"
    "src/tools/profiler.cpp"
    "src/ui/card.cpp"
    "src/ui/cursor.cpp"
    "src/ui/groupbox.cpp"
    "src/ui/modal.cpp"
    "src/ui/sidebar.cpp"
    "src/ui/splash_screen.cpp"
    "src/ui/subtabs.cpp"
    "src/ui/titlebar.cpp"
    "src/ui/tooltip.cpp"
    "src/ui/welcome_screen.cpp"
    "src/ui/window.cpp"
    "src/ui/satellite_window.cpp"
    "src/ui/kill_banner.cpp"
    "src/ui/command_palette.cpp"
    "src/widgets/accordion.cpp"
    "src/widgets/badge.cpp"
    "src/widgets/button.cpp"
    "src/widgets/checkbox.cpp"
    "src/widgets/color_picker.cpp"
    "src/widgets/combo.cpp"
    "src/widgets/dropdown_multi.cpp"
    "src/widgets/input_text.cpp"
    "src/widgets/keybind.cpp"
    "src/widgets/knob_slider.cpp"
    "src/widgets/multi_combo.cpp"
    "src/widgets/number_stepper.cpp"
    "src/widgets/performance_graph.cpp"
    "src/widgets/progress_bar.cpp"
    "src/widgets/radar.cpp"
    "src/widgets/chip_selector.cpp"
    "src/widgets/radial_gauge.cpp"
    "src/widgets/range_slider.cpp"
    "src/widgets/search_input.cpp"
    "src/widgets/segmented_control.cpp"
    "src/widgets/separator.cpp"
    "src/widgets/slider.cpp"
    "src/widgets/stat_card.cpp"
    "src/widgets/toggle.cpp"
)

add_library(solar_lib STATIC
    ${SOLAR_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(solar_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

target_compile_definitions(solar_lib PUBLIC
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

# Standalone DirectX 11 + Win32 Demo Executable
set(DEMO_SOURCES
    "demo/main.cpp"
    "demo/demo_app.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_win32.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
)

add_executable(solar_demo WIN32
    ${DEMO_SOURCES}
)

add_executable(bin2c
    "${IMGUI_DIR}/misc/fonts/binary_to_compressed_c.cpp"
)

target_link_libraries(solar_demo PRIVATE
    solar_lib
    d3d11
    dxgi
    dwmapi
    winmm
    shcore
)

target_include_directories(solar_demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/demo"
)

# SolarWorld: Standalone 3D DirectX 11 testbed emulating Unreal Engine internal structures
add_executable(solar_world WIN32
    "tools/solar_world/main.cpp"
)

target_link_libraries(solar_world PRIVATE
    d3d11
    dxgi
    d3dcompiler
    dwmapi
    winmm
)

