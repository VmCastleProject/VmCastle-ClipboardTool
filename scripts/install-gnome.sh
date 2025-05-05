#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Clipboard Manager Installation for GNOME ===${NC}"

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

# Copy the binary
echo -e "${YELLOW}Installing binary...${NC}"
cp -f build/clipboard_manager ~/.local/bin/

# Create systemd service file
echo -e "${YELLOW}Creating systemd service...${NC}"
cat > ~/.config/systemd/user/clipboard-manager.service << EOF
[Unit]
Description=Clipboard Manager Service
After=gnome-session.target
PartOf=gnome-session.target

[Service]
Type=simple
ExecStart=/home/$USER/.local/bin/clipboard_manager
Restart=on-failure
RestartSec=5s
Environment=DISPLAY=:0
Environment=XAUTHORITY=%h/.Xauthority
Environment=DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$(id -u)/bus

[Install]
WantedBy=gnome-session.target
EOF

# Create a desktop file
echo -e "${YELLOW}Creating desktop file...${NC}"
cat > ~/.local/share/applications/clipboard-manager.desktop << EOF
[Desktop Entry]
Name=Clipboard Manager
Comment=A lightweight clipboard manager
Exec=/home/$USER/.local/bin/clipboard_manager
Icon=edit-paste
Terminal=false
Type=Application
Categories=Utility;GTK;
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

# Set up GNOME keybinding
echo -e "${YELLOW}Setting up GNOME keybinding...${NC}"

# First check if gsettings is available
if command -v gsettings >/dev/null 2>&1; then
    # Get current custom keybindings
    CURRENT_BINDINGS=$(gsettings get org.gnome.settings-daemon.plugins.media-keys custom-keybindings)
    
    # If no bindings exist, create a new array
    if [[ "$CURRENT_BINDINGS" == "@as []" ]]; then
        NEW_BINDING="['/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/clipboard-manager/']"
        gsettings set org.gnome.settings-daemon.plugins.media-keys custom-keybindings "$NEW_BINDING"
    else
        # Check if our binding already exists
        if ! echo "$CURRENT_BINDINGS" | grep -q "clipboard-manager"; then
            # Add our binding to the existing array
            NEW_BINDING=$(echo "$CURRENT_BINDINGS" | sed -e "s/]/, \\'\//org\\/gnome\\/settings-daemon\\/plugins\\/media-keys\\/custom-keybindings\\/clipboard-manager\\/\\']/")
            gsettings set org.gnome.settings-daemon.plugins.media-keys custom-keybindings "$NEW_BINDING"
        fi
    fi
    
    # Create the custom keybinding
    gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/clipboard-manager/ name "Clipboard Manager"
    gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/clipboard-manager/ command "/home/$USER/.local/bin/toggle-clipboard.sh"
    gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/clipboard-manager/ binding "<Super>v"
    
    echo -e "${GREEN}GNOME keybinding set up successfully.${NC}"
else
    echo -e "${RED}gsettings command not found. You'll need to set up the keyboard shortcut manually:${NC}"
    echo -e "${YELLOW}1. Open Settings > Keyboard Shortcuts${NC}"
    echo -e "${YELLOW}2. Add a custom shortcut with:${NC}"
    echo -e "${YELLOW}   Name: Clipboard Manager${NC}"
    echo -e "${YELLOW}   Command: /home/$USER/.local/bin/toggle-clipboard.sh${NC}"
    echo -e "${YELLOW}   Shortcut: Super+V${NC}"
fi

# Enable and start the service
echo -e "${YELLOW}Enabling and starting service...${NC}"
systemctl --user daemon-reload
systemctl --user enable clipboard-manager.service
systemctl --user restart clipboard-manager.service

echo -e "${GREEN}Installation complete!${NC}"
echo -e "${YELLOW}Press SUPER+V to toggle the clipboard manager.${NC}"
echo -e "${YELLOW}You can check the status with: systemctl --user status clipboard-manager.service${NC}"
