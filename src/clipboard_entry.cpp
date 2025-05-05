// Copyright (C) 2025 Vinícius (VmCastle)
// Este arquivo é parte de um software licenciado sob a GPLv3.
// Consulte o arquivo LICENSE para mais informações.

#include "clipboard_entry.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

ClipboardEntry::ClipboardEntry(const std::string& text)
    : text_(text), timestamp_(std::time(nullptr)) {
}

const std::string& ClipboardEntry::get_text() const {
    return text_;
}

std::string ClipboardEntry::get_preview(size_t max_length) const {
    if (text_.length() <= max_length) {
        return text_;
    }
    
    // Truncate and add ellipsis
    return text_.substr(0, max_length - 3) + "...";
}

std::time_t ClipboardEntry::get_timestamp() const {
    return timestamp_;
}

std::string ClipboardEntry::get_formatted_time() const {
    std::tm* tm = std::localtime(&timestamp_);
    std::ostringstream oss;
    oss << std::put_time(tm, "%H:%M:%S");
    return oss.str();
}

size_t ClipboardEntry::get_size() const {
    return text_.size();
}
