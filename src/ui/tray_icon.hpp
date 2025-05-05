// Copyright (C) 2025 Vinícius (VmCastle)
// Este arquivo é parte de um software licenciado sob a GPLv3.
// Consulte o arquivo LICENSE para mais informações.


#ifndef TRAY_ICON_HPP
#define TRAY_ICON_HPP

#include <gtk/gtk.h>
#include <memory>
#include "../clipboard_manager.hpp"

G_BEGIN_DECLS

#define TRAY_ICON_TYPE (tray_icon_get_type())
G_DECLARE_FINAL_TYPE(TrayIcon, tray_icon, TRAY, ICON, GObject)

// Create a new tray icon
TrayIcon* tray_icon_new(std::shared_ptr<ClipboardManager> manager, GtkWindow* main_window);

// Show the tray icon
void tray_icon_show(TrayIcon* tray);

// Hide the tray icon
void tray_icon_hide(TrayIcon* tray);

G_END_DECLS

#endif // TRAY_ICON_HPP
