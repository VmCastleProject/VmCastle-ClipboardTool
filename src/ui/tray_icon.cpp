/**
 * TrayIcon class implementation.
 * Manages a small floating window that acts as a system tray icon.
 * Note: GTK4 has removed GtkStatusIcon, so we implement a small floating window instead.
 */

 #include "tray_icon.hpp"
 #include "main_window.hpp"
 #include "../clipboard_manager.hpp"
 #include <iostream>
 
 struct _TrayIcon {
     GObject parent_instance;
     
     // Icon window (acts as the tray icon)
     GtkWidget* window;
     GtkWidget* button;
     
     // Popover menu
     GtkWidget* menu;
     GtkWidget* popover;
     
     // Menu items
     GtkWidget* recent_items_box;
     GtkWidget* show_item;
     GtkWidget* quit_item;
     
     // Data
     std::shared_ptr<ClipboardManager> clipboard_manager;
     GtkWindow* main_window;
     
     // State
     gboolean visible;
 };
 
 G_DEFINE_TYPE(TrayIcon, tray_icon, G_TYPE_OBJECT)
 
 // Forward declarations
 static void create_menu(TrayIcon* tray);
 static void on_show_clicked(GtkButton* button, gpointer user_data);
 static void on_quit_clicked(GtkButton* button, gpointer user_data);
 static void on_button_clicked(GtkButton* button, gpointer user_data);
 static void update_recent_items_menu(TrayIcon* tray);
 static void on_recent_item_clicked(GtkButton* button, gpointer user_data);
 static void position_window_on_screen(TrayIcon* tray);
 
 static void tray_icon_dispose(GObject* object) {
     TrayIcon* tray = TRAY_ICON(object);
     
     // Clear clipboard manager reference
     tray->clipboard_manager = nullptr;
     
     // Unreference main window
     if (tray->main_window) {
         g_object_unref(tray->main_window);
         tray->main_window = nullptr;
     }
     
     // Chain up to parent
     G_OBJECT_CLASS(tray_icon_parent_class)->dispose(object);
 }
 
 static void tray_icon_finalize(GObject* object) {
     TrayIcon* tray = TRAY_ICON(object);
     
     // Destroy window
     if (tray->window) {
         gtk_window_destroy(GTK_WINDOW(tray->window));
     }
     
     // Chain up to parent
     G_OBJECT_CLASS(tray_icon_parent_class)->finalize(object);
 }
 
 static void tray_icon_class_init(TrayIconClass* klass) {
     GObjectClass* object_class = G_OBJECT_CLASS(klass);
     object_class->dispose = tray_icon_dispose;
     object_class->finalize = tray_icon_finalize;
 }
 
 static void tray_icon_init(TrayIcon* tray) {
     tray->visible = FALSE;
     
     // Create floating window to act as a tray icon
     tray->window = gtk_window_new();
     gtk_window_set_title(GTK_WINDOW(tray->window), "Clipboard Manager");
     gtk_window_set_decorated(GTK_WINDOW(tray->window), FALSE);  // No window decorations
     // Note: In GTK4, we use GtkWindow properties instead of set_skip_taskbar_hint
     g_object_set(G_OBJECT(tray->window), "deletable", FALSE, NULL);  // Not closable by window manager
     gtk_window_set_default_size(GTK_WINDOW(tray->window), 32, 32);  // Small size
     
     // Create icon button
     tray->button = gtk_button_new_from_icon_name("edit-paste");  // Clipboard icon
     gtk_button_set_has_frame(GTK_BUTTON(tray->button), FALSE);  // No button frame
     gtk_widget_set_tooltip_text(tray->button, "Clipboard Manager");
     
     // Connect button signal
     g_signal_connect(tray->button, "clicked", G_CALLBACK(on_button_clicked), tray);
     
     // Add button to window
     gtk_window_set_child(GTK_WINDOW(tray->window), tray->button);
 }
 
 TrayIcon* tray_icon_new(std::shared_ptr<ClipboardManager> manager, GtkWindow* main_window) {
     TrayIcon* tray = TRAY_ICON(g_object_new(TRAY_ICON_TYPE, NULL));
     
     // Store clipboard manager
     tray->clipboard_manager = manager;
     
     // Store main window
     tray->main_window = GTK_WINDOW(g_object_ref(main_window));
     
     // Create menu and popover
     create_menu(tray);
     
     // Position window on screen
     position_window_on_screen(tray);
     
     // Register for clipboard changes
     manager->register_callback([tray]() {
         g_idle_add(+[](gpointer user_data) -> gboolean {
             update_recent_items_menu(TRAY_ICON(user_data));
             return G_SOURCE_REMOVE;
         }, tray);
     });
     
     return tray;
 }
 
 void tray_icon_show(TrayIcon* tray) {
     tray->visible = TRUE;
     gtk_widget_set_visible(tray->window, TRUE);
 }
 
 void tray_icon_hide(TrayIcon* tray) {
     tray->visible = FALSE;
     gtk_widget_set_visible(tray->window, FALSE);
 }
 
 static void position_window_on_screen(TrayIcon* tray) {
     // Get the display
     GdkDisplay* display = gdk_display_get_default();
     
     if (!display) {
         return;  // No display available
     }
     
     // In GTK4, we use surface APIs to position windows
     // Set a fixed position for the tray icon in the bottom-right corner
     // We cannot access monitor directly in GTK4, so use fixed values for demo
     
     // Get the surface of the window
     GdkSurface* surface = gtk_native_get_surface(GTK_NATIVE(tray->window));
     if (surface) {
         // The window needs to be visible to position it
         // We'll set a fixed position at the bottom-right corner (e.g., 100px from right and bottom edges)
         // In a real application, we would determine the screen size programmatically
         gtk_window_present(GTK_WINDOW(tray->window));
         
         // Set a fixed bottom-right position for now
         // In a real app, we'd calculate this based on screen dimensions
         gtk_widget_set_margin_end(GTK_WIDGET(tray->window), 10);
         gtk_widget_set_margin_bottom(GTK_WIDGET(tray->window), 10);
     }
 }
 
 static void create_menu(TrayIcon* tray) {
     // Create a box for the menu
     tray->menu = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
     gtk_widget_set_margin_start(tray->menu, 12);
     gtk_widget_set_margin_end(tray->menu, 12);
     gtk_widget_set_margin_top(tray->menu, 12);
     gtk_widget_set_margin_bottom(tray->menu, 12);
     
     // Create a label for recent items
     GtkWidget* recent_label = gtk_label_new("Recent Items");
     gtk_widget_set_halign(recent_label, GTK_ALIGN_START);
     gtk_widget_add_css_class(recent_label, "heading");
     
     // Create box for recent items
     tray->recent_items_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
     
     // Separator
     GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
     
     // Show manager button
     tray->show_item = gtk_button_new_with_label("Show Manager");
     g_signal_connect(tray->show_item, "clicked", G_CALLBACK(on_show_clicked), tray);
     
     // Quit button
     tray->quit_item = gtk_button_new_with_label("Quit");
     g_signal_connect(tray->quit_item, "clicked", G_CALLBACK(on_quit_clicked), tray);
     
     // Add everything to the menu box
     gtk_box_append(GTK_BOX(tray->menu), recent_label);
     gtk_box_append(GTK_BOX(tray->menu), tray->recent_items_box);
     gtk_box_append(GTK_BOX(tray->menu), separator);
     gtk_box_append(GTK_BOX(tray->menu), tray->show_item);
     gtk_box_append(GTK_BOX(tray->menu), tray->quit_item);
     
     // Create popover for the menu
     tray->popover = gtk_popover_new();
     gtk_popover_set_child(GTK_POPOVER(tray->popover), tray->menu);
     gtk_popover_set_has_arrow(GTK_POPOVER(tray->popover), FALSE);
     gtk_widget_set_parent(tray->popover, tray->window);
     
     // Populate recent items
     update_recent_items_menu(tray);
 }
 
 static void update_recent_items_menu(TrayIcon* tray) {
     // Remove existing items
     GtkWidget* child;
     while ((child = gtk_widget_get_first_child(tray->recent_items_box)) != NULL) {
         // Use gtk_box_remove instead of manually setting parent to NULL
         gtk_box_remove(GTK_BOX(tray->recent_items_box), child);
     }
     
     // Get entries
     auto entries = tray->clipboard_manager->get_entries();
     
     // Add up to 10 recent items
     const size_t max_recent = 10;
     size_t count = std::min(entries.size(), max_recent);
     
     if (count == 0) {
         // Add a placeholder message when there are no entries
         GtkWidget* empty_label = gtk_label_new("No clipboard entries yet");
         gtk_widget_set_sensitive(empty_label, FALSE);
         gtk_widget_set_halign(empty_label, GTK_ALIGN_START);
         gtk_box_append(GTK_BOX(tray->recent_items_box), empty_label);
     }
     
     for (size_t i = 0; i < count; ++i) {
         const auto& entry = entries[i];
         
         // Create button with preview text
         GtkWidget* btn = gtk_button_new_with_label(entry->get_preview(30).c_str());
         gtk_widget_set_halign(btn, GTK_ALIGN_FILL);
         gtk_button_set_has_frame(GTK_BUTTON(btn), FALSE);
         
         // Store entry index
         g_object_set_data(G_OBJECT(btn), "entry-index", GINT_TO_POINTER(i));
         
         // Connect signal
         g_signal_connect(btn, "clicked", G_CALLBACK(on_recent_item_clicked), tray);
         
         // Add to menu
         gtk_box_append(GTK_BOX(tray->recent_items_box), btn);
     }
 }
 
 static void on_show_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
     TrayIcon* tray = TRAY_ICON(user_data);
     
     // Hide the popover
     gtk_popover_popdown(GTK_POPOVER(tray->popover));
     
     // Show main window
     main_window_toggle_visibility(MAIN_WINDOW(tray->main_window));
 }
 
 static void on_quit_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
     TrayIcon* tray = TRAY_ICON(user_data);
     
     // Hide the popover
     gtk_popover_popdown(GTK_POPOVER(tray->popover));
     
     // Quit application
     g_application_quit(G_APPLICATION(gtk_window_get_application(tray->main_window)));
 }
 
 static void on_button_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
     TrayIcon* tray = TRAY_ICON(user_data);
     
     // Update recent items before showing menu
     update_recent_items_menu(tray);
     
     // Show the popover menu
     gtk_popover_popup(GTK_POPOVER(tray->popover));
 }
 
 static void on_recent_item_clicked(GtkButton* button, gpointer user_data) {
     TrayIcon* tray = TRAY_ICON(user_data);
     
     // Hide the popover
     gtk_popover_popdown(GTK_POPOVER(tray->popover));
     
     // Get entry index
     gint index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "entry-index"));
     
     // Copy to clipboard
     tray->clipboard_manager->copy_to_clipboard(index);
 }
 