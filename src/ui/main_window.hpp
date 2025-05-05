// Copyright (C) 2025 Vinícius (VmCastle)
// Este arquivo é parte de um software licenciado sob a GPLv3.
// Consulte o arquivo LICENSE para mais informações.


#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <gtk/gtk.h>
#include <memory>
#include "../clipboard_manager.hpp"

G_BEGIN_DECLS

#define MAIN_WINDOW_TYPE (main_window_get_type())
G_DECLARE_FINAL_TYPE(MainWindow, main_window, MAIN, WINDOW, GtkApplicationWindow)

// Create a new main window
MainWindow* main_window_new(GtkApplication* app, std::shared_ptr<ClipboardManager> manager);

// Refresh the clipboard entries display
void main_window_refresh(MainWindow* window);

// Show or hide the main window
void main_window_toggle_visibility(MainWindow* window);

G_END_DECLS

#endif // MAIN_WINDOW_HPP
