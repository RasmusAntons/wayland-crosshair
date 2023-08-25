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

## Installation

Copy `wayland_crosshair` and `toggle-crosshair` into the PATH, e.g. `~/bin`.

On KDE, go to System Settings -> Shortcuts -> Add Command, enter `toggle-crosshair` and add a custom shortcut.

## Note

I only tested this on KDE Plasma 5. For me, the crosshair is always in the center of the currently active screen.
I don't know how it behaves on different window managers or scaling options.
