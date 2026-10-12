# Vara

## Overview
- `Vara` is a modern, lightweight, tabbed, and split-pane terminal emulator written in C using GTK 4 and VTE.

## Installation
1. Clone the repository:
```bash
git clone https://github.com/nhrx1337/Vara.git
cd Vara/
```
2. Install build dependencies:
### Debian-based
```bash
sudo apt-get install libgtk-4-dev libvte-2.91-gtk4-dev meson ninja-build
```
### Arch-based
```bash
sudo pacman -S gtk4 vte4 meson ninja
```
3. Configure and build with Meson:
```bash
meson setup build --buildtype=release
meson compile -C build
# or install system-wide
sudo meson install -C build
```

## Keybindings
- Ctrl + Shift + T: Open a new tab
- Ctrl + Shift + E: Split terminal horizontally
- Ctrl + Shift + W: Split terminal vertically
- Ctrl + Shift + Left / Right: Switch between tabs
- Ctrl + Shift + 1-9: Jump directly to tab 1 through 9
- Ctrl + Shift + P: Increase font size
- Ctrl + Shift + O: Decrease font size
- Ctrl + Shift + I: Reset font size
- Ctrl + Shift + C: Copy to clipboard
- Ctrl + Shift + V: Paste from clipboard
- Ctrl + Shift + M: Toggle tab bar visibility

## Dependencies
- GTK 4
- VTE (vte-2.91-gtk4)
- Meson & Ninja

## Uninstallation
- If you installed system-wide via Meson:
```bash
sudo ninja -C build uninstall
```
Or simply clean/remove the build folder:
```bash
rm -rf build
```
