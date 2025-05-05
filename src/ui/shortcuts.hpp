// Copyright (C) 2025 Vinícius (VmCastle)
// Este arquivo é parte de um software licenciado sob a GPLv3.
// Consulte o arquivo LICENSE para mais informações.


#ifndef SHORTCUTS_HPP
#define SHORTCUTS_HPP

#include <gtk/gtk.h>
#include <memory>
#include "../clipboard_manager.hpp"

// Initialize shortcuts
void shortcuts_init(GtkApplication* app, std::shared_ptr<ClipboardManager> manager, GtkWindow* main_window);

// Release shortcuts resources
void shortcuts_cleanup();

#endif // SHORTCUTS_HPP
