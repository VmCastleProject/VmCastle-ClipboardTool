/**
 * Main entry point for the clipboard manager application.
 * Initializes GTK, creates the clipboard manager, and runs the main loop.
 */

 #include <gtk/gtk.h>
 #include <iostream>
 #include <memory>
 
 // Include order matters to avoid circular dependencies
 #include "clipboard_manager.hpp"
 #include "ui/main_window.hpp"
 #include "ui/shortcuts.hpp"
 // No longer using separate tray icon window
 
 int main(int argc, char* argv[]) {
     // Initialize GTK
     gtk_init();
     
     // Create the clipboard manager
     auto clipboard_manager = std::make_shared<ClipboardManager>();
     
     // Create the application
     GtkApplication* app = gtk_application_new("org.example.clipboard_manager", G_APPLICATION_DEFAULT_FLAGS);
     
     g_signal_connect(app, "activate", G_CALLBACK(+[](GtkApplication* app, gpointer user_data) {
         auto manager = static_cast<std::shared_ptr<ClipboardManager>*>(user_data);
         
         // Create main window (we're not using separate tray icon anymore)
         MainWindow* window = main_window_new(app, *manager);
         
         // Add a header bar with app menu
         GtkWidget* header_bar = gtk_header_bar_new();
         gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);
         
         // Add app menu button
         GtkWidget* menu_button = gtk_menu_button_new();
         gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(menu_button), "open-menu-symbolic");
         gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), menu_button);
         
         // Create popover menu for the app menu
         GtkWidget* popover = gtk_popover_new();
         gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu_button), popover);
         
         // Create box for menu items
         GtkWidget* menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
         gtk_widget_set_margin_start(menu_box, 12);
         gtk_widget_set_margin_end(menu_box, 12);
         gtk_widget_set_margin_top(menu_box, 12);
         gtk_widget_set_margin_bottom(menu_box, 12);
         
         // Add menu items
         GtkWidget* about_item = gtk_button_new_with_label("About");
         gtk_button_set_has_frame(GTK_BUTTON(about_item), FALSE);
         g_signal_connect(about_item, "clicked", G_CALLBACK(+[](GtkButton* button, gpointer user_data) {
             GtkWindow* window = GTK_WINDOW(user_data);
             
             // Create about dialog
             GtkWidget* dialog = gtk_about_dialog_new();
             gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Clipboard Manager");
             gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
             gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "© 2023");
             gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
                 "A lightweight clipboard manager for Arch Linux.\n"
                 "Uses GTK4 and xclip for clipboard operations.");
             gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
             gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
             
             // Show dialog
             gtk_widget_set_visible(dialog, TRUE);
         }), window);
         
         GtkWidget* quit_item = gtk_button_new_with_label("Quit");
         gtk_button_set_has_frame(GTK_BUTTON(quit_item), FALSE);
         g_signal_connect(quit_item, "clicked", G_CALLBACK(+[](GtkButton* button, gpointer user_data) {
             GApplication* app = G_APPLICATION(user_data);
             g_application_quit(app);
         }), app);
         
         // Add items to menu box
         gtk_box_append(GTK_BOX(menu_box), about_item);
         gtk_box_append(GTK_BOX(menu_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
         gtk_box_append(GTK_BOX(menu_box), quit_item);
         
         // Set menu box as popover child
         gtk_popover_set_child(GTK_POPOVER(popover), menu_box);
         
         // Show the window
         gtk_window_present(GTK_WINDOW(window));
         
         // Initialize shortcuts
         shortcuts_init(app, *manager, GTK_WINDOW(window));
         
     }), &clipboard_manager);
     
     // Start clipboard monitoring
     clipboard_manager->start_monitoring();
     
     // Run the application
     int status = g_application_run(G_APPLICATION(app), argc, argv);
     
     // Cleanup
     shortcuts_cleanup();
     g_object_unref(app);
     
     return status;
 }
 