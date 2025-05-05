#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Clipboard Manager Installation (Generic) ===${NC}"

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
mkdir -p ~/.config/autostart

# Copy the binary
echo -e "${YELLOW}Installing binary...${NC}"
cp -f build/clipboard_manager ~/.local/bin/

# Create systemd service file
echo -e "${YELLOW}Creating systemd service...${NC}"
cat > ~/.config/systemd/user/clipboard-manager.service << EOF
[Unit]
Description=Clipboard Manager Service
After=graphical-session.target
PartOf=graphical-session.target

[Service]
Type=simple
ExecStart=/home/$USER/.local/bin/clipboard_manager
Restart=on-failure
RestartSec=5s
Environment=DISPLAY=:0
Environment=XAUTHORITY=%h/.Xauthority
Environment=DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$(id -u)/bus
Environment=XDG_RUNTIME_DIR=/run/user/$(id -u)

[Install]
WantedBy=graphical-session.target
EOF

# Create an autostart file
echo -e "${YELLOW}Creating autostart file...${NC}"
cat > ~/.config/autostart/clipboard-manager.desktop << EOF
[Desktop Entry]
Name=Clipboard Manager
Comment=A lightweight clipboard manager
Exec=/home/$USER/.local/bin/clipboard_manager
Icon=edit-paste
Terminal=false
Type=Application
Categories=Utility;
StartupNotify=false
X-GNOME-Autostart-enabled=true
EOF

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

echo -e "${YELLOW}Setting up manual keyboard shortcut...${NC}"
echo -e "${YELLOW}Since your desktop environment is not specifically supported, you'll need to set up the keyboard shortcut manually:${NC}"
echo -e "${YELLOW}1. In your desktop environment's keyboard settings, add a new shortcut for:${NC}"
echo -e "${YELLOW}   Command: /home/$USER/.local/bin/toggle-clipboard.sh${NC}"
echo -e "${YELLOW}   Shortcut: Super+V (or any key combination you prefer)${NC}"

# Enable and start the service
echo -e "${YELLOW}Enabling and starting service...${NC}"
systemctl --user daemon-reload
systemctl --user enable clipboard-manager.service
systemctl --user restart clipboard-manager.service

echo -e "${GREEN}Installation complete!${NC}"
echo -e "${YELLOW}The clipboard manager service is now running in the background.${NC}"
echo -e "${YELLOW}Remember to set up your keyboard shortcut manually to toggle the clipboard manager.${NC}"
echo -e "${YELLOW}You can check the status with: systemctl --user status clipboard-manager.service${NC}"
