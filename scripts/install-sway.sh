#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Clipboard Manager Installation for Sway ===${NC}"

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
mkdir -p ~/.config/sway

# Copy the binary
echo -e "${YELLOW}Installing binary...${NC}"
cp -f build/clipboard_manager ~/.local/bin/

# Create systemd service file
echo -e "${YELLOW}Creating systemd service...${NC}"
cat > ~/.config/systemd/user/clipboard-manager.service << EOF
[Unit]
Description=Clipboard Manager Service
After=sway-session.target
PartOf=graphical-session.target

[Service]
Type=simple
ExecStart=/home/$USER/.local/bin/clipboard_manager
Restart=on-failure
RestartSec=5s
Environment=WAYLAND_DISPLAY=wayland-1
Environment=XDG_CURRENT_DESKTOP=sway
Environment=GDK_BACKEND=wayland

[Install]
WantedBy=sway-session.target
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

# Configure Sway keybinding
echo -e "${YELLOW}Adding Sway keybinding...${NC}"

# Check if the config file exists
CONFIG_FILE="~/.config/sway/config"
if [ -f ~/.config/sway/config ]; then
    # Check if the keybind already exists
    if grep -q "toggle-clipboard.sh" ~/.config/sway/config; then
        echo -e "${YELLOW}Keybinding already exists in Sway config.${NC}"
    else
        # Add the keybinding if it doesn't exist
        echo "# Clipboard Manager" >> ~/.config/sway/config
        echo "bindsym Mod4+v exec ~/.local/bin/toggle-clipboard.sh" >> ~/.config/sway/config
        echo -e "${GREEN}Added keybinding to Sway config.${NC}"
    fi
else
    echo -e "${RED}Sway config file not found. Add this line manually to your Sway config:${NC}"
    echo -e "${YELLOW}bindsym Mod4+v exec ~/.local/bin/toggle-clipboard.sh${NC}"
fi

# Enable and start the service
echo -e "${YELLOW}Enabling and starting service...${NC}"
systemctl --user daemon-reload
systemctl --user enable clipboard-manager.service
systemctl --user restart clipboard-manager.service

echo -e "${GREEN}Installation complete!${NC}"
echo -e "${YELLOW}Press SUPER+V to toggle the clipboard manager.${NC}"
echo -e "${YELLOW}You may need to reload Sway (default: SUPER+Shift+c) for the keybind to take effect.${NC}"
echo -e "${YELLOW}You can check the status with: systemctl --user status clipboard-manager.service${NC}"
