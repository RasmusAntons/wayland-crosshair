cmake_minimum_required(VERSION 3.26)
project(wayland_crosshair C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS -DWLR_USE_UNSTABLE)

execute_process(COMMAND pkg-config --variable=wayland_scanner wayland-scanner OUTPUT_VARIABLE WAYLAND_SCANNER OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND pkg-config wayland-protocols --variable=pkgdatadir OUTPUT_VARIABLE WAYLAND_PROTOCOLS_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/xdg-shell-client-protocol.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND ${WAYLAND_SCANNER} client-header "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml" xdg-shell-client-protocol.h
)
add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/xdg-shell-protocol.c
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND ${WAYLAND_SCANNER} private-code "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml" xdg-shell-protocol.c
)
add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/wlr-layer-shell-unstable-v1-client-protocol.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND ${WAYLAND_SCANNER} client-header "protocol/wlr-layer-shell-unstable-v1.xml" wlr-layer-shell-unstable-v1-client-protocol.h
)
add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/wlr-layer-shell-unstable-v1-client-protocol.c
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND ${WAYLAND_SCANNER} private-code "protocol/wlr-layer-shell-unstable-v1.xml" wlr-layer-shell-unstable-v1-client-protocol.c
)

include(FindPkgConfig)
pkg_check_modules(WLCLIENT REQUIRED wayland-client)
include_directories(${WLCLIENT_INCLUDE_DIRS})
pkg_check_modules(WLCLIENT REQUIRED wlroots)
include_directories(${WLROOTS_INCLUDE_DIRS})
pkg_check_modules(WLEGL REQUIRED wayland-egl)
include_directories(${WLEGL_INCLUDE_DIRS})

add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/crosshair.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND convert crosshair.png -define h:format=bgra -depth 8 crosshair.h
)

add_executable(
		wayland_crosshair main.c crosshair.h
		shm.c xdg-shell-protocol.c wlr-layer-shell-unstable-v1-client-protocol.c
		shm.h xdg-shell-client-protocol.h wlr-layer-shell-unstable-v1-client-protocol.h
)
target_link_libraries(wayland_crosshair ${WLCLIENT_LDFLAGS} ${WLROOTS_LDFLAGS} ${WLEGL_LDFLAGS} rt)
