/**
 * MainWindow class implementation.
 * Manages the main application window UI.
 */

 #include "main_window.hpp"
 #include "../clipboard_manager.hpp"
 #include <iostream>
 
 struct _MainWindow {
     GtkApplicationWindow parent_instance;
     
     // UI elements
     GtkWidget* list_box;
     GtkWidget* search_entry;
     GtkWidget* clear_button;
     
     // Data
     std::shared_ptr<ClipboardManager> clipboard_manager;
 };
 
 G_DEFINE_TYPE(MainWindow, main_window, GTK_TYPE_APPLICATION_WINDOW)
 
 // Forward declarations
 static void create_ui(MainWindow* window);
 static void setup_signals(MainWindow* window);
 static void populate_list(MainWindow* window);
 static void on_row_activated(GtkListBox* list_box, GtkListBoxRow* row, gpointer user_data);
 static void on_clear_clicked(GtkButton* button, gpointer user_data);
 static void on_search_changed(GtkSearchEntry* entry, gpointer user_data);
 static void on_delete_entry(GtkButton* button, gpointer user_data);
 
 static void main_window_dispose(GObject* object) {
     MainWindow* window = MAIN_WINDOW(object);
     
     // Clear clipboard manager reference
     window->clipboard_manager = nullptr;
     
     // Chain up to parent
     G_OBJECT_CLASS(main_window_parent_class)->dispose(object);
 }
 
 static void main_window_class_init(MainWindowClass* klass) {
     GObjectClass* object_class = G_OBJECT_CLASS(klass);
     object_class->dispose = main_window_dispose;
 }
 
 static void main_window_init(MainWindow* window) {
     // Set default size
     gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
     gtk_window_set_title(GTK_WINDOW(window), "Clipboard Manager");
     gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
     
     // Set window as initially hidden
     gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
     
     // Hide on close
     g_signal_connect(window, "close-request", G_CALLBACK(+[](GtkWindow* window, gpointer user_data G_GNUC_UNUSED) {
         // Hide instead of destroy
         gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
         return TRUE;
     }), NULL);
 }
 
 MainWindow* main_window_new(GtkApplication* app, std::shared_ptr<ClipboardManager> manager) {
     MainWindow* window = MAIN_WINDOW(g_object_new(MAIN_WINDOW_TYPE,
                                                 "application", app,
                                                 NULL));
     
     // Store clipboard manager
     window->clipboard_manager = manager;
     
     // Create UI
     create_ui(window);
     
     // Setup signals
     setup_signals(window);
     
     // Populate list
     populate_list(window);
     
     // Register for clipboard changes
     manager->register_callback([window]() {
         g_idle_add(+[](gpointer user_data) -> gboolean {
             main_window_refresh(MAIN_WINDOW(user_data));
             return G_SOURCE_REMOVE;
         }, window);
     });
     
     return window;
 }
 
 void main_window_refresh(MainWindow* window) {
     populate_list(window);
 }
 
 void main_window_toggle_visibility(MainWindow* window) {
     gboolean visible = gtk_widget_get_visible(GTK_WIDGET(window));
     gtk_widget_set_visible(GTK_WIDGET(window), !visible);
     
     if (!visible) {
         gtk_window_present(GTK_WINDOW(window));
     }
 }
 
 static void create_ui(MainWindow* window) {
     // Main box
     GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
     gtk_widget_set_margin_start(main_box, 12);
     gtk_widget_set_margin_end(main_box, 12);
     gtk_widget_set_margin_top(main_box, 12);
     gtk_widget_set_margin_bottom(main_box, 12);
     
     // Header with search and clear button
     GtkWidget* header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
     window->search_entry = gtk_search_entry_new();
     gtk_widget_set_hexpand(window->search_entry, TRUE);
     window->clear_button = gtk_button_new_with_label("Clear All");
     
     gtk_box_append(GTK_BOX(header_box), window->search_entry);
     gtk_box_append(GTK_BOX(header_box), window->clear_button);
     
     // Add quick access section with label
     GtkWidget* quick_label = gtk_label_new("Recent Items");
     gtk_widget_set_halign(quick_label, GTK_ALIGN_START);
     gtk_widget_add_css_class(quick_label, "heading");
     
     // Recent items flow box (horizontal)
     GtkWidget* recent_box = gtk_flow_box_new();
     gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(recent_box), GTK_SELECTION_SINGLE);
     gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(recent_box), 10);
     gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(recent_box), 6);
     gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(recent_box), 6);
     gtk_widget_set_margin_bottom(recent_box, 6);
     
     // Add 5 recent item buttons (they'll be updated later)
     const size_t max_quick_items = 5;
     for (size_t i = 0; i < max_quick_items; i++) {
         GtkWidget* btn = gtk_button_new_with_label("--");
         gtk_flow_box_insert(GTK_FLOW_BOX(recent_box), btn, -1);
         gtk_widget_set_sensitive(btn, FALSE); // Disabled initially
     }
     
     // History section label
     GtkWidget* history_label = gtk_label_new("Clipboard History");
     gtk_widget_set_halign(history_label, GTK_ALIGN_START);
     gtk_widget_add_css_class(history_label, "heading");
     
     // Scrolled window for list
     GtkWidget* scrolled = gtk_scrolled_window_new();
     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
     gtk_widget_set_vexpand(scrolled, TRUE);
     
     // List box for entries
     window->list_box = gtk_list_box_new();
     gtk_list_box_set_selection_mode(GTK_LIST_BOX(window->list_box), GTK_SELECTION_SINGLE);
     gtk_widget_add_css_class(window->list_box, "rich-list");
     
     // Add list to scrolled window
     gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), window->list_box);
     
     // Add everything to main box
     gtk_box_append(GTK_BOX(main_box), header_box);
     gtk_box_append(GTK_BOX(main_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
     gtk_box_append(GTK_BOX(main_box), quick_label);
     gtk_box_append(GTK_BOX(main_box), recent_box);
     gtk_box_append(GTK_BOX(main_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
     gtk_box_append(GTK_BOX(main_box), history_label);
     gtk_box_append(GTK_BOX(main_box), scrolled);
     
     // Store reference to recent_box for updates
     g_object_set_data(G_OBJECT(window), "recent-box", recent_box);
     
     // Set main box as window content
     gtk_window_set_child(GTK_WINDOW(window), main_box);
 }
 
 static void setup_signals(MainWindow* window) {
     // Row activation
     g_signal_connect(window->list_box, "row-activated", 
                     G_CALLBACK(on_row_activated), window);
     
     // Clear button
     g_signal_connect(window->clear_button, "clicked", 
                     G_CALLBACK(on_clear_clicked), window);
     
     // Search entry
     g_signal_connect(window->search_entry, "search-changed", 
                     G_CALLBACK(on_search_changed), window);
 }
 
 static void populate_list(MainWindow* window) {
     // Remove existing rows
     GtkWidget* child;
     while ((child = gtk_widget_get_first_child(window->list_box)) != NULL) {
         gtk_list_box_remove(GTK_LIST_BOX(window->list_box), child);
     }
     
     // Get entries
     auto entries = window->clipboard_manager->get_entries();
     
     // Get search text
     const char* search_text = gtk_editable_get_text(GTK_EDITABLE(window->search_entry));
     std::string filter = search_text ? search_text : "";
     
     // Update quick access buttons first
     GtkWidget* recent_box = GTK_WIDGET(g_object_get_data(G_OBJECT(window), "recent-box"));
     if (recent_box) {
         // Get the flow box children
         const size_t max_quick_items = 5;
         
         // For each button in the flow box
         GtkWidget* child_widget;
         size_t button_index = 0;
         GtkFlowBoxChild* flow_child = gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(recent_box), button_index);
         
         while (flow_child && button_index < max_quick_items) {
             // Get the child widget (button)
             child_widget = gtk_flow_box_child_get_child(flow_child);
             
             // If we have an entry for this index
             if (button_index < entries.size()) {
                 const auto& entry = entries[button_index];
                 
                 // Update the button with preview text
                 std::string preview = entry->get_preview();
                 if (preview.length() > 15) {
                     preview = preview.substr(0, 12) + "...";
                 }
                 
                 gtk_button_set_label(GTK_BUTTON(child_widget), preview.c_str());
                 gtk_widget_set_sensitive(child_widget, TRUE);
                 
                 // Connect or update click handler
                 g_signal_handlers_disconnect_by_data(child_widget, window);
                 g_object_set_data(G_OBJECT(child_widget), "entry-index", GINT_TO_POINTER(button_index));
                 g_signal_connect(child_widget, "clicked", G_CALLBACK(+[](GtkButton* button, gpointer user_data) {
                     MainWindow* window = MAIN_WINDOW(user_data);
                     gint index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "entry-index"));
                     
                     // Copy to clipboard
                     if (window->clipboard_manager->copy_to_clipboard(index)) {
                         // Hide window after copying
                         gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
                     }
                 }), window);
             } else {
                 // No entry for this button
                 gtk_button_set_label(GTK_BUTTON(child_widget), "--");
                 gtk_widget_set_sensitive(child_widget, FALSE);
                 g_signal_handlers_disconnect_by_data(child_widget, window);
             }
             
             // Move to next button
             button_index++;
             flow_child = gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(recent_box), button_index);
         }
     }
     
     // Add entries to main list
     for (size_t i = 0; i < entries.size(); ++i) {
         const auto& entry = entries[i];
         
         // Apply filter if search text is present
         if (!filter.empty()) {
             if (entry->get_text().find(filter) == std::string::npos) {
                 continue;
             }
         }
         
         // Create row
         GtkWidget* row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
         gtk_widget_set_margin_start(row_box, 6);
         gtk_widget_set_margin_end(row_box, 6);
         gtk_widget_set_margin_top(row_box, 6);
         gtk_widget_set_margin_bottom(row_box, 6);
         
         // Label with preview text
         GtkWidget* label = gtk_label_new(entry->get_preview().c_str());
         gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
         gtk_widget_set_hexpand(label, TRUE);
         gtk_widget_set_halign(label, GTK_ALIGN_START);
         
         // Time label
         std::string time_str = entry->get_formatted_time();
         GtkWidget* time_label = gtk_label_new(time_str.c_str());
         gtk_widget_add_css_class(time_label, "dim-label");
         
         // Delete button
         GtkWidget* delete_button = gtk_button_new_from_icon_name("edit-delete-symbolic");
         gtk_button_set_has_frame(GTK_BUTTON(delete_button), FALSE);
         g_object_set_data(G_OBJECT(delete_button), "entry-index", GINT_TO_POINTER(i));
         g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_entry), window);
         
         // Add widgets to row
         gtk_box_append(GTK_BOX(row_box), label);
         gtk_box_append(GTK_BOX(row_box), time_label);
         gtk_box_append(GTK_BOX(row_box), delete_button);
         
         // Add row to list
         gtk_list_box_insert(GTK_LIST_BOX(window->list_box), row_box, -1);
         
         // Store entry index
         GtkListBoxRow* list_row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(window->list_box), i);
         g_object_set_data(G_OBJECT(list_row), "entry-index", GINT_TO_POINTER(i));
     }
 }
 
 static void on_row_activated(GtkListBox* list_box G_GNUC_UNUSED, GtkListBoxRow* row, gpointer user_data) {
     MainWindow* window = MAIN_WINDOW(user_data);
     
     // Get entry index
     gint index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(row), "entry-index"));
     
     // Copy to clipboard
     if (window->clipboard_manager->copy_to_clipboard(index)) {
         // Hide window after copying
         gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
     }
 }
 
 static void on_clear_clicked(GtkButton* button G_GNUC_UNUSED, gpointer user_data) {
     MainWindow* window = MAIN_WINDOW(user_data);
     
     // Create a dialog using newer GTK4 approach
     GtkWidget* dialog = gtk_dialog_new_with_buttons("Confirm Clear",
                                        GTK_WINDOW(window),
                                        static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                        "_Clear", GTK_RESPONSE_YES,
                                        NULL);
     
     // Create content for the dialog
     GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
     GtkWidget* label = gtk_label_new("Clear all clipboard entries?");
     gtk_widget_set_margin_start(label, 12);
     gtk_widget_set_margin_end(label, 12);
     gtk_widget_set_margin_top(label, 12);
     gtk_widget_set_margin_bottom(label, 12);
     gtk_box_append(GTK_BOX(content_area), label);
     
     // Set minimum size
     gtk_widget_set_size_request(dialog, 300, -1);
     
     // Set up dialog response handler
     g_signal_connect(dialog, "response", G_CALLBACK(+[](GtkDialog* dialog, gint response, gpointer user_data){
         MainWindow* window = MAIN_WINDOW(user_data);
         
         if (response == GTK_RESPONSE_YES) {
             // Clear entries
             window->clipboard_manager->clear_entries();
             populate_list(window);
         }
         
         // Destroy dialog
         gtk_window_destroy(GTK_WINDOW(dialog));
     }), window);
     
     // Show dialog
     gtk_widget_set_visible(dialog, TRUE);
 }
 
 static void on_search_changed(GtkSearchEntry* entry G_GNUC_UNUSED, gpointer user_data) {
     MainWindow* window = MAIN_WINDOW(user_data);
     
     // Refresh list to apply filter
     populate_list(window);
 }
 
 static void on_delete_entry(GtkButton* button, gpointer user_data) {
     MainWindow* window = MAIN_WINDOW(user_data);
     
     // Get entry index
     gint index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "entry-index"));
     
     // Remove entry
     window->clipboard_manager->remove_entry(index);
     
     // Refresh list
     populate_list(window);
 }
 