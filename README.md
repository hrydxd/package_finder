# pckgfind - Lightweight Package Finder

A simple, fast, and colorful command-line tool to search for packages in your Linux distribution.

## Features

- 🔍 **Fast search** - searches only package names (not descriptions)
- 🎨 **Colorful output** - packages, versions, and repositories are color-coded
- 📦 **Multi-distro support** - works with APT, Pacman, DNF, and Zypper
- 💾 **Lightweight** - written in C, no external dependencies
- 🚀 **Easy to use** - just type `pckgfind <package_name>`

## Supported Package Managers

| Distribution | Package Manager |
|--------------|-----------------|
| Debian/Ubuntu | APT |
| Arch Linux | Pacman |
| Fedora | DNF |
| openSUSE | Zypper |

## Installation

### Quick Install

```bash
git clone https://github.com/yourusername/pckgfind.git
cd pckgfind
chmod +x install.sh
./install.sh
