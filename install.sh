#!/bin/bash

echo "[VmCastle Installer - Arch Linux]"
echo "Installing dependencies..."

# Install GTKmm and xclip if needed
sudo pacman -Sy --noconfirm base-devel cmake gtkmm3 xclip

#if you're using windows or any other distro, it maybe doesn't run, so take this code and convert with AI to use it by your terminal...

echo "Building project..."
mkdir -p build
cd build
cmake ..
make

echo "Installation complete. Run it with: ./build/vmcastle"
