#ifndef CLIPBOARD_MANAGER_H
#define CLIPBOARD_MANAGER_H

#include <iostream>
#include <vector>
#include <string>

class ClipboardManager {
public:
    ClipboardManager();
    void checkForNewClipboardContent();
    void printWelcomeMessage(); 
    void printClipboardHistory();
    void addToHistory(const std::string& content);  
    std::vector<std::string> clipboardHistory;
    std::string historyFilePath;
    std::string lastClipboardContent;
    void loadHistoryFromFile();
    void saveHistoryToFile();
    std::string getClipboard(); 
};

#endif 
