#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Clipboard Manager Installation for KDE Plasma ===${NC}"

# Check for dependencies
echo -e "${YELLOW}Checking dependencies...${NC}"
deps=(cmake gcc pkg-config gtk4 xclip)
missing_deps=()

for dep in "${deps[@]}"; do
  if ! pacman -Q $dep &> /dev/null; then
    missing_deps+=("$dep")
  fi
done

if [ ${#missing_deps[@]} -ne 0 ]; then
  echo -e "${RED}Missing dependencies: ${missing_deps[*]}${NC}"
  echo -e "${YELLOW}Installing missing dependencies...${NC}"
  sudo pacman -S --needed ${missing_deps[*]}
fi

# Build the application
echo -e "${YELLOW}Building clipboard manager...${NC}"
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..

# Create installation directories
echo -e "${YELLOW}Setting up application...${NC}"
mkdir -p ~/.local/bin
mkdir -p ~/.config/systemd/user
mkdir -p ~/.local/share/applications
mkdir -p ~/.config/autostart

# Copy the binary
echo -e "${YELLOW}Installing binary...${NC}"
cp -f build/clipboard_manager ~/.local/bin/

# Create systemd service file
echo -e "${YELLOW}Creating systemd service...${NC}"
cat > ~/.config/systemd/user/clipboard-manager.service << EOF
[Unit]
Description=Clipboard Manager Service
After=plasma-core.target
PartOf=graphical-session.target

[Service]
Type=simple
ExecStart=/home/$USER/.local/bin/clipboard_manager
Restart=on-failure
RestartSec=5s
Environment=DISPLAY=:0
Environment=XAUTHORITY=%h/.Xauthority
Environment=DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$(id -u)/bus

[Install]
WantedBy=plasma-core.target
EOF

# Create a desktop file for autostart
echo -e "${YELLOW}Creating desktop and autostart files...${NC}"
cat > ~/.local/share/applications/clipboard-manager.desktop << EOF
[Desktop Entry]
Name=Clipboard Manager
Comment=A lightweight clipboard manager
Exec=/home/$USER/.local/bin/clipboard_manager
Icon=edit-paste
Terminal=false
Type=Application
Categories=Utility;Qt;
StartupNotify=false
EOF

# Create symbolic link for autostart
ln -sf ~/.local/share/applications/clipboard-manager.desktop ~/.config/autostart/

# Create a toggle script
echo -e "${YELLOW}Creating toggle script...${NC}"
cat > ~/.local/bin/toggle-clipboard.sh << EOF
#!/bin/bash

# Check if the process is running
if pidof clipboard_manager > /dev/null; then
    # Send USR1 signal to toggle visibility
    pkill -USR1 -f clipboard_manager
else
    # Start the clipboard manager
    /home/$USER/.local/bin/clipboard_manager &
fi
EOF

chmod +x ~/.local/bin/toggle-clipboard.sh

# Attempt to set up KDE shortcuts
echo -e "${YELLOW}Setting up KDE keyboard shortcut...${NC}"

# Check if we can use kwriteconfig5
if command -v kwriteconfig5 >/dev/null 2>&1; then
    # Create a new shortcut for clipboard manager
    SHORTCUT_COUNT=$(kreadconfig5 --group "Global Shortcuts" --key "_k_friendly_name" | wc -l)
    NEXT_INDEX=$((SHORTCUT_COUNT + 1))
    
    kwriteconfig5 --file kglobalshortcutsrc --group "clipboard-manager-shortcut" --key "_k_friendly_name" "Clipboard Manager"
    kwriteconfig5 --file kglobalshortcutsrc --group "clipboard-manager-shortcut" --key "toggle-clipboard" "/home/$USER/.local/bin/toggle-clipboard.sh,Meta+V,Toggle Clipboard Manager"
    
    echo -e "${GREEN}KDE keyboard shortcut created. You may need to reload KWin or log out and back in.${NC}"
    echo -e "${YELLOW}If the shortcut doesn't work, set it manually through System Settings > Shortcuts.${NC}"
else
    echo -e "${RED}Cannot automatically set up KDE shortcuts. Please set up manually:${NC}"
    echo -e "${YELLOW}1. Open System Settings > Shortcuts > Custom Shortcuts${NC}"
    echo -e "${YELLOW}2. Create a new shortcut with:${NC}"
    echo -e "${YELLOW}   Name: Clipboard Manager${NC}"
    echo -e "${YELLOW}   Command: /home/$USER/.local/bin/toggle-clipboard.sh${NC}"
    echo -e "${YELLOW}   Shortcut: Meta+V${NC}"
fi

# Enable and start the service
echo -e "${YELLOW}Enabling and starting service...${NC}"
systemctl --user daemon-reload
systemctl --user enable clipboard-manager.service
systemctl --user restart clipboard-manager.service

echo -e "${GREEN}Installation complete!${NC}"
echo -e "${YELLOW}Press SUPER+V to toggle the clipboard manager.${NC}"
echo -e "${YELLOW}You can check the status with: systemctl --user status clipboard-manager.service${NC}"
