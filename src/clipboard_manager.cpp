/**
 * ClipboardManager class implementation.
 * Manages the clipboard entries and provides access to clipboard functionality.
 * Uses xclip for clipboard operations on Arch Linux.
 */

 #include "clipboard_manager.hpp"
 #include <iostream>
 #include <cstdio>
 #include <cstdlib>
 #include <array>
 #include <memory>
 #include <unistd.h>
 #include <fcntl.h>
 #include <string.h>
 #include <cerrno>
 
 // Define the static constant
 const size_t ClipboardManager::MAX_ENTRIES;
 
 ClipboardManager::ClipboardManager()
     : clipboard_(nullptr), updating_clipboard_(false), monitor_source_id_(0) {
     // Get default display for GTK functionality
     GdkDisplay* display = gdk_display_get_default();
     if (display) {
         // Get clipboard for default display (still needed for GTK UI integration)
         clipboard_ = gdk_display_get_clipboard(display);
     }
 }
 
 ClipboardManager::~ClipboardManager() {
     // Stop monitoring
     stop_monitoring();
     
     // Save clipboard history before closing
     save_history_to_file();
 }
 
 void ClipboardManager::start_monitoring() {
     // Use xclip-based monitoring for Arch Linux
     start_xclip_monitoring();
 }
 
 void ClipboardManager::stop_monitoring() {
     stop_xclip_monitoring();
 }
 
 void ClipboardManager::start_xclip_monitoring() {
     // Check if already monitoring
     if (monitor_source_id_ != 0) {
         return;
     }
     
     // Try to load existing clipboard history from saved file if exists
     load_history_from_file();
     
     // Try to get initial clipboard content
     last_clipboard_content_ = execute_xclip("-o -selection clipboard 2>/dev/null");
     
     // If we got content from clipboard, add it to history
     if (!last_clipboard_content_.empty()) {
         add_entry(last_clipboard_content_);
     }
     
     // Set up a timer to check for clipboard changes
     // Check every 500ms (adjust as needed for performance vs responsiveness)
     monitor_source_id_ = g_timeout_add(500, check_clipboard_changes, this);
 }
 
 void ClipboardManager::stop_xclip_monitoring() {
     if (monitor_source_id_ != 0) {
         g_source_remove(monitor_source_id_);
         monitor_source_id_ = 0;
     }
 }
 
 std::vector<std::shared_ptr<ClipboardEntry>> ClipboardManager::get_entries() const {
     std::lock_guard<std::mutex> lock(mutex_);
     return entries_;
 }
 
 std::shared_ptr<ClipboardEntry> ClipboardManager::get_entry(size_t index) const {
     std::lock_guard<std::mutex> lock(mutex_);
     if (index < entries_.size()) {
         return entries_[index];
     }
     return nullptr;
 }
 
 size_t ClipboardManager::get_entry_count() const {
     std::lock_guard<std::mutex> lock(mutex_);
     return entries_.size();
 }
 
 bool ClipboardManager::copy_to_clipboard(size_t index) {
     std::lock_guard<std::mutex> lock(mutex_);
     if (index >= entries_.size()) {
         return false;
     }
     
     // Set updating flag to prevent recursive clipboard changes
     updating_clipboard_ = true;
     
     // Get the entry text
     const std::string& text = entries_[index]->get_text();
     
     try {
         // Create a temporary file to store the text
         char temp_filename[] = "/tmp/clipboard_manager_XXXXXX";
         int fd = mkstemp(temp_filename);
         
         if (fd == -1) {
             std::cerr << "Error creating temporary file" << std::endl;
             updating_clipboard_ = false;
             return false;
         }
         
         // Write text to the file
         FILE* file = fdopen(fd, "w");
         if (!file) {
             close(fd);
             std::cerr << "Error opening temporary file" << std::endl;
             updating_clipboard_ = false;
             return false;
         }
         
         fwrite(text.c_str(), sizeof(char), text.length(), file);
         fclose(file);
         
         // Use xclip to set both clipboard and primary selection content
         // Redirect stderr to /dev/null to suppress errors
         std::string clipboard_cmd = "cat " + std::string(temp_filename) + " | xclip -selection clipboard -i 2>/dev/null";
         std::string primary_cmd = "cat " + std::string(temp_filename) + " | xclip -selection primary -i 2>/dev/null";
         
         // Copy to clipboard selection
         int clipboard_result = system(clipboard_cmd.c_str());
         
         // Copy to primary selection as well for X apps that use primary selection
         int primary_result = system(primary_cmd.c_str());
         
         // At least one should succeed
         int result = (clipboard_result == 0 || primary_result == 0) ? 0 : -1;
         
         // Clean up temporary file
         unlink(temp_filename);
         
         if (result != 0) {
             std::cerr << "Error running xclip command" << std::endl;
             updating_clipboard_ = false;
             return false;
         }
     } catch (const std::exception& e) {
         std::cerr << "Exception in copy_to_clipboard: " << e.what() << std::endl;
         updating_clipboard_ = false;
         return false;
     }
     
     // Update last clipboard content
     last_clipboard_content_ = text;
     
     // Move the copied entry to the front
     auto entry = entries_[index];
     entries_.erase(entries_.begin() + index);
     entries_.insert(entries_.begin(), entry);
     
     updating_clipboard_ = false;
     return true;
 }
 
 void ClipboardManager::clear_entries() {
     std::lock_guard<std::mutex> lock(mutex_);
     entries_.clear();
     notify_callbacks();
 }
 
 void ClipboardManager::remove_entry(size_t index) {
     std::lock_guard<std::mutex> lock(mutex_);
     if (index < entries_.size()) {
         entries_.erase(entries_.begin() + index);
         notify_callbacks();
     }
 }
 
 void ClipboardManager::register_callback(ClipboardChangedCallback callback) {
     std::lock_guard<std::mutex> lock(mutex_);
     callbacks_.push_back(callback);
 }
 
 void ClipboardManager::on_clipboard_changed(GdkClipboard* clipboard, gpointer user_data) {
     ClipboardManager* self = static_cast<ClipboardManager*>(user_data);
     
     // Prevent recursive updates
     if (self->updating_clipboard_) {
         return;
     }
     
     // Request text from clipboard
     gdk_clipboard_read_text_async(clipboard, nullptr, 
         [](GObject* source_object, GAsyncResult* res, gpointer user_data) {
             ClipboardManager* self = static_cast<ClipboardManager*>(user_data);
             GdkClipboard* clipboard = GDK_CLIPBOARD(source_object);
             
             char* text = gdk_clipboard_read_text_finish(clipboard, res, nullptr);
             if (text) {
                 // Add to clipboard entries
                 self->add_entry(text);
                 g_free(text);
             }
         }, self);
 }
 
 void ClipboardManager::add_entry(const std::string& text) {
     // Don't add empty text
     if (text.empty()) {
         return;
     }
     
     std::lock_guard<std::mutex> lock(mutex_);
     
     // Check if text already exists
     // Use manual loop instead of std::find_if for better compiler compatibility
     auto it = entries_.end();
     for (auto iter = entries_.begin(); iter != entries_.end(); ++iter) {
         if ((*iter)->get_text() == text) {
             it = iter;
             break;
         }
     }
     
     if (it != entries_.end()) {
         // Move existing entry to front
         auto entry = *it;
         entries_.erase(it);
         entries_.insert(entries_.begin(), entry);
     } else {
         // Create new entry
         auto new_entry = std::make_shared<ClipboardEntry>(text);
         entries_.insert(entries_.begin(), new_entry);
         
         // Limit the number of entries
         if (entries_.size() > MAX_ENTRIES) {
             entries_.pop_back();
         }
     }
     
     // Notify callbacks
     notify_callbacks();
 }
 
 void ClipboardManager::notify_callbacks() {
     for (const auto& callback : callbacks_) {
         callback();
     }
 }
 
 std::string ClipboardManager::execute_xclip(const std::string& args) {
     std::string command = "xclip " + args;
     std::array<char, 128> buffer;
     std::string result;
     
     // Prevent recursion
     if (updating_clipboard_) {
         return result;
     }
     
     try {
         // Open pipe to command
         // Use a raw pointer approach to avoid the attributes warning
         FILE* raw_pipe = popen(command.c_str(), "r");
         if (!raw_pipe) {
             std::cerr << "Error executing xclip command: " << command << std::endl;
             return result;
         }
         
         // Set up auto-cleanup with RAII
         struct PipeCloser {
             FILE* pipe;
             ~PipeCloser() { if (pipe) pclose(pipe); }
         } pipe_closer{raw_pipe};
         
         // Read output
         while (fgets(buffer.data(), buffer.size(), raw_pipe) != nullptr) {
             result += buffer.data();
         }
     } catch (const std::exception& e) {
         std::cerr << "Exception executing xclip: " << e.what() << std::endl;
     }
     
     return result;
 }
 
 gboolean ClipboardManager::check_clipboard_changes(gpointer user_data) {
     ClipboardManager* self = static_cast<ClipboardManager*>(user_data);
     
     // Prevent recursive updates
     if (self->updating_clipboard_) {
         return G_SOURCE_CONTINUE;
     }
     
     // Get current clipboard content
     // Redirect stderr to /dev/null to suppress error messages
     // Try both primary and clipboard selections to ensure we catch all changes
     std::string current_content = self->execute_xclip("-o -selection clipboard 2>/dev/null");
     
     // If clipboard selection is empty, try primary selection as fallback
     if (current_content.empty()) {
         current_content = self->execute_xclip("-o -selection primary 2>/dev/null");
     }
     
     // If content has changed and is not empty
     if (!current_content.empty() && current_content != self->last_clipboard_content_) {
         // Update last content
         self->last_clipboard_content_ = current_content;
         
         // Add to entries
         self->add_entry(current_content);
     }
     
     // Continue monitoring
     return G_SOURCE_CONTINUE;
 }
 
 void ClipboardManager::load_history_from_file() {
     // File path in user's home directory
     const char* home_dir = getenv("HOME");
     if (!home_dir) {
         std::cerr << "Could not get HOME directory for history file" << std::endl;
         return;
     }
     
     std::string history_file = std::string(home_dir) + "/.clipboard_history";
     
     // Try to open the file
     FILE* file = fopen(history_file.c_str(), "r");
     if (!file) {
         // It's okay if the file doesn't exist yet
         return;
     }
     
     // Read entries line by line
     char buffer[4096];
     std::string current_entry;
     bool in_entry = false;
     
     while (fgets(buffer, sizeof(buffer), file)) {
         std::string line(buffer);
         
         // Remove trailing newline
         if (!line.empty() && line[line.length()-1] == '\n') {
             line.erase(line.length()-1);
         }
         
         // Check for entry markers
         if (line == "---ENTRY_START---") {
             in_entry = true;
             current_entry.clear();
         } else if (line == "---ENTRY_END---") {
             if (in_entry && !current_entry.empty()) {
                 // Add this entry
                 add_entry(current_entry);
             }
             in_entry = false;
         } else if (in_entry) {
             // Add line to current entry
             if (!current_entry.empty()) {
                 current_entry += "\n";
             }
             current_entry += line;
         }
     }
     
     fclose(file);
 }
 
 void ClipboardManager::save_history_to_file() {
     std::lock_guard<std::mutex> lock(mutex_);
     
     // Don't save if there are no entries
     if (entries_.empty()) {
         return;
     }
     
     // File path in user's home directory
     const char* home_dir = getenv("HOME");
     if (!home_dir) {
         std::cerr << "Could not get HOME directory for history file" << std::endl;
         return;
     }
     
     std::string history_file = std::string(home_dir) + "/.clipboard_history";
     
     // Open the file for writing
     FILE* file = fopen(history_file.c_str(), "w");
     if (!file) {
         std::cerr << "Could not open history file for writing: " << history_file << std::endl;
         return;
     }
     
     // Write entries (up to MAX_ENTRIES)
     size_t count = std::min(entries_.size(), MAX_ENTRIES);
     for (size_t i = 0; i < count; ++i) {
         const auto& entry = entries_[i];
         fprintf(file, "---ENTRY_START---\n");
         fprintf(file, "%s\n", entry->get_text().c_str());
         fprintf(file, "---ENTRY_END---\n");
     }
     
     fclose(file);
 }
 