/**
 * Shortcuts module implementation.
 * Manages global keyboard shortcuts for the clipboard manager.
 */

 #include "shortcuts.hpp"
 #include "main_window.hpp"
 #include <iostream>
 
 // Static variables
 static GtkApplication* app_instance = nullptr;
 static std::shared_ptr<ClipboardManager> clipboard_manager_instance = nullptr;
 static GtkWindow* main_window_instance = nullptr;
 
 // Forward declarations
 static void on_toggle_window_shortcut(GSimpleAction* action, GVariant* parameter, gpointer user_data);
 static void on_paste_recent_shortcut(GSimpleAction* action, GVariant* parameter, gpointer user_data);
 
 void shortcuts_init(GtkApplication* app, std::shared_ptr<ClipboardManager> manager, GtkWindow* main_window) {
     // Store instances
     app_instance = app;
     clipboard_manager_instance = manager;
     main_window_instance = main_window;
     
     // Create actions
     GSimpleAction* toggle_window_action = g_simple_action_new("toggle-window", NULL);
     GSimpleAction* paste_recent_action = g_simple_action_new("paste-recent", NULL);
     
     // Connect signals
     g_signal_connect(toggle_window_action, "activate", G_CALLBACK(on_toggle_window_shortcut), NULL);
     g_signal_connect(paste_recent_action, "activate", G_CALLBACK(on_paste_recent_shortcut), NULL);
     
     // Add actions to application
     g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(toggle_window_action));
     g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(paste_recent_action));
     
     // Set accelerators
     const char* toggle_window_accelerators[] = {"<Control><Alt>c", NULL};
     const char* paste_recent_accelerators[] = {"<Control><Alt>v", NULL};
     
     gtk_application_set_accels_for_action(app, "app.toggle-window", toggle_window_accelerators);
     gtk_application_set_accels_for_action(app, "app.paste-recent", paste_recent_accelerators);
 }
 
 void shortcuts_cleanup() {
     // Clear references
     app_instance = nullptr;
     clipboard_manager_instance = nullptr;
     main_window_instance = nullptr;
 }
 
 static void on_toggle_window_shortcut(GSimpleAction* action, GVariant* parameter, gpointer user_data) {
     if (main_window_instance) {
         main_window_toggle_visibility(MAIN_WINDOW(main_window_instance));
     }
 }
 
 static void on_paste_recent_shortcut(GSimpleAction* action, GVariant* parameter, gpointer user_data) {
     if (clipboard_manager_instance) {
         // Get the most recent entry (index 0) and copy it to clipboard
         if (clipboard_manager_instance->get_entry_count() > 0) {
             clipboard_manager_instance->copy_to_clipboard(0);
         }
     }
 }
 