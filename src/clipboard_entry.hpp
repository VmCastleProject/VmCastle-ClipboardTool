// Copyright (C) 2025 Vinícius (VmCastle)
// Este arquivo é parte de um software licenciado sob a GPLv3.
// Consulte o arquivo LICENSE para mais informações.


#ifndef CLIPBOARD_ENTRY_HPP
#define CLIPBOARD_ENTRY_HPP

#include <string>
#include <ctime>

class ClipboardEntry {
public:
    // Constructor
    explicit ClipboardEntry(const std::string& text);
    
    // Get the text content
    const std::string& get_text() const;
    
    // Get preview text (truncated if too long)
    std::string get_preview(size_t max_length = 50) const;
    
    // Get timestamp when entry was created
    std::time_t get_timestamp() const;
    
    // Get timestamp as formatted string
    std::string get_formatted_time() const;
    
    // Get size in bytes
    size_t get_size() const;
    
private:
    std::string text_;        // The clipboard text content
    std::time_t timestamp_;   // When the entry was created
};

#endif // CLIPBOARD_ENTRY_HPP
