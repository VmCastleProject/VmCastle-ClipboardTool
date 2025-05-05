# VmCastle Installation Scripts

This directory contains scripts to help you install and set up the clipboard manager on different desktop environments and window managers.

## Available Installation Scripts

- `install-hyprland.sh` - For Hyprland window manager
- `install-i3.sh` - For i3 window manager
- `install-gnome.sh` - For GNOME desktop environment
- `install-kde.sh` - For KDE Plasma desktop environment
- `install-sway.sh` - For Sway window manager
- `install-generic.sh` - For any other desktop environment/window manager

## How to Install

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/clipboard-manager.git
   cd clipboard-manager
   ```

2. Make the installation script executable:
   ```bash
   chmod +x scripts/install-YOUR-DE.sh
   ```
   Replace `YOUR-DE` with your desktop environment (hyprland, i3, gnome, kde, sway, or generic)

3. Run the installation script:
   ```bash
   ./scripts/install-YOUR-DE.sh
   ```

4. Follow the instructions displayed by the script.

## What the Scripts Do

Each installation script performs the following tasks:

1. Checks for and installs required dependencies
2. Builds the application from source
3. Installs the binary to `~/.local/bin/`
4. Creates a systemd user service for background operation
5. Sets up the appropriate keyboard shortcut for your desktop environment (SUPER+V)
6. Enables and starts the service

## Manual Configuration

If you need to configure the clipboard manager manually:

1. Build the application:
   ```bash
   mkdir -p build
   cd build
   cmake ..
   make
   ```

2. Copy the binary to a location in your PATH:
   ```bash
   cp build/clipboard_manager ~/.local/bin/
   ```

3. Create a systemd user service:
   ```bash
   mkdir -p ~/.config/systemd/user/
   ```

   Create a file at `~/.config/systemd/user/clipboard-manager.service` with content appropriate for your desktop environment (see the installation scripts for examples).

4. Set up a keyboard shortcut in your desktop environment to run:
   ```bash
   ~/.local/bin/toggle-clipboard.sh
   ```

5. Enable and start the service:
   ```bash
   systemctl --user daemon-reload
   systemctl --user enable clipboard-manager.service
   systemctl --user start clipboard-manager.service
   ```

## Troubleshooting

If you encounter issues:

1. Check the service status:
   ```bash
   systemctl --user status clipboard-manager.service
   ```

2. View logs for errors:
   ```bash
   journalctl --user -u clipboard-manager.service
   ```

3. Try running the application directly to see any error messages:
   ```bash
   ~/.local/bin/clipboard_manager
   ```

## Uninstalling

To uninstall:

1. Stop and disable the service:
   ```bash
   systemctl --user stop clipboard-manager.service
   systemctl --user disable clipboard-manager.service
   ```

2. Remove the files:
   ```bash
   rm ~/.local/bin/clipboard_manager
   rm ~/.local/bin/toggle-clipboard.sh
   rm ~/.config/systemd/user/clipboard-manager.service
   ```

3. Remove any keyboard shortcuts you added

4. Optional: Remove clipboard history:
   ```bash
   rm -rf ~/.clipboard_history
   ```
