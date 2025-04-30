#include "clipboard_manager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <array>
#include <stdexcept>
#include <memory>

ClipboardManager::ClipboardManager() { // gerencia o que foi e o que será colado, instancia o caminho do txt
    historyFilePath = std::string(std::getenv("HOME")) + "/.clipboard_history.txt";
    loadHistoryFromFile();
    if (!clipboardHistory.empty()) {
        lastClipboardContent = clipboardHistory.back(); 
    }
}

std::string ClipboardManager::getClipboard() { // usa a ferramenta xclip para pegar o que for copiado
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("xclip -o -selection clipboard 2>/dev/null", "r"), pclose);
    if (!pipe) {
        std::cerr << "Error: Unable to open pipe." << std::endl;
        return "";
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    if (result.empty()) {
        std::cerr << "Error: Clipboard is empty or could not be read." << std::endl;
    }
    return result;
}

void ClipboardManager::addToHistory(const std::string& content) { // adiciona no histórico 
    if (clipboardHistory.size() >= 10) {
        clipboardHistory.erase(clipboardHistory.begin()); 
    }
    clipboardHistory.push_back(content);
}

void ClipboardManager::printWelcomeMessage() { //estrutura a ordem de apresentação dos dados pelo terminal
    std::cout << "Welcome to VmCastle\n\n";
    std::cout << "Clipboard History:\n";
    if (clipboardHistory.empty()) {
        std::cout << "- No clipboard history available\n";
    } else {
        for (const auto& entry : clipboardHistory) {
            std::cout << "- " << entry << "\n";
        }
    }
    std::cout << "\n➔ [1] Buscar itens no histórico por palavra-chave\n";
}

void ClipboardManager::saveHistoryToFile() { // salva o que foi copiado em um txt
    std::ofstream file(historyFilePath, std::ios::trunc);
    if (file.is_open()) {
        for (const auto& entry : clipboardHistory) {
            file << entry << "\n===ENTRY_END===\n"; 
        }
        file.close();
    }
}

void ClipboardManager::loadHistoryFromFile() { //carrega o histórico de coisas copiadas e armazenados em txt
    std::ifstream file(historyFilePath);
    if (file.is_open()) {
        std::string line, entry;
        while (std::getline(file, line)) {
            if (line == "===ENTRY_END===") {
                if (!entry.empty()) {
                    clipboardHistory.push_back(entry);
                    entry.clear();
                }
            } else {
                entry += line + "\n";
            }
        }
        file.close();
    }
}

void ClipboardManager::checkForNewClipboardContent() { //função para verificar se um novo ctrl + c foi dado
    try {
        std::string content = getClipboard();
        if (content != lastClipboardContent && !content.empty()) { 
            lastClipboardContent = content;
            addToHistory(content);
            saveHistoryToFile();
            std::cout << content << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void ClipboardManager::printClipboardHistory() { //printa o histórico via terminal
    if (clipboardHistory.empty()) {
        std::cout << "No clipboard history available.\n";
    } else {
        std::cout << "Clipboard History:\n";
        for (const auto& entry : clipboardHistory) {
            std::cout << "- " << entry;
        }
    }
}
