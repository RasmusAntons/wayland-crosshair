# Wayland-Crosshair

A simple crosshair overlay for wayland using wlroots [zwlr_layer_shell_v1](https://wayland.app/protocols/wlr-layer-shell-unstable-v1).
This code is an adaptation of wlroots [examples/layer-shell.c](https://gitlab.freedesktop.org/wlroots/wlroots/-/blob/master/examples/layer-shell.c) and [hello-wayland](https://github.com/emersion/hello-wayland).

## Requirements

Build: cmake pkgconf imagemagick wayland wayland-protocols wlroots

Run: wayland wlroots

## Building

```bash
mkdir cmake-build
cd cmake-build
cmake ..
make
```
