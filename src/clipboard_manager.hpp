/**
 * ClipboardManager class declaration.
 * Manages the clipboard entries and provides access to clipboard functionality.
 */

 #ifndef CLIPBOARD_MANAGER_HPP
 #define CLIPBOARD_MANAGER_HPP
 
 #include <gtk/gtk.h>
 #include <vector>
 #include <memory>
 #include <functional>
 #include <mutex>
 
 #include "clipboard_entry.hpp"
 
 class ClipboardManager {
 public:
     // Maximum number of entries to store
     static const size_t MAX_ENTRIES = 50;
     
     // Constructor and destructor
     ClipboardManager();
     ~ClipboardManager();
     
     // Clipboard operations
     void start_monitoring();
     void stop_monitoring();
     
     // Get all entries
     std::vector<std::shared_ptr<ClipboardEntry>> get_entries() const;
     
     // Get entry at index
     std::shared_ptr<ClipboardEntry> get_entry(size_t index) const;
     
     // Get the number of entries
     size_t get_entry_count() const;
     
     // Copy entry at index to system clipboard
     bool copy_to_clipboard(size_t index);
     
     // Clear all entries
     void clear_entries();
     
     // Remove entry at index
     void remove_entry(size_t index);
     
     // Register callback for clipboard changes
     using ClipboardChangedCallback = std::function<void()>;
     void register_callback(ClipboardChangedCallback callback);
     
 private:
     // Monitor clipboard using xclip
     void start_xclip_monitoring();
     void stop_xclip_monitoring();
     
     // Clipboard content change handler
     static void on_clipboard_changed(GdkClipboard* clipboard, gpointer user_data);
     
     // Execute xclip command and get output
     std::string execute_xclip(const std::string& args);
     
     // Check clipboard for changes
     static gboolean check_clipboard_changes(gpointer user_data);
     
     // Add new entry
     void add_entry(const std::string& text);
     
     // Notify callbacks
     void notify_callbacks();
     
     // Load clipboard history from file
     void load_history_from_file();
     
     // Save clipboard history to file
     void save_history_to_file();
     
     // System clipboard
     GdkClipboard* clipboard_;
     
     // Clipboard entries
     std::vector<std::shared_ptr<ClipboardEntry>> entries_;
     
     // Mutex for thread safety
     mutable std::mutex mutex_;
     
     // Callbacks for clipboard changes
     std::vector<ClipboardChangedCallback> callbacks_;
     
     // Flag to prevent recursive clipboard changes
     bool updating_clipboard_;
     
     // Last clipboard content for change detection
     std::string last_clipboard_content_;
     
     // Monitoring source ID for xclip
     guint monitor_source_id_;
 };
 
 #endif // CLIPBOARD_MANAGER_HPP
 