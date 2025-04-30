#ifndef GUI_H
#define GUI_H

#include <gtkmm.h>
#include <vector>

class GuiWindow : public Gtk::Window {
public:
    GuiWindow();
    virtual ~GuiWindow() {}

private:
    void startClipboardMonitoring();
    void updateClipboardHistory();
    void addClipboardItem(const Glib::ustring& text);
    void onCopyButtonClicked(const Glib::ustring& text);

    Gtk::Box m_mainBox{Gtk::ORIENTATION_VERTICAL};  
    Gtk::ScrolledWindow m_scrolledWindow;
    Gtk::ListBox m_listBox;  
    std::vector<Glib::ustring> clipboardHistory;
};

#endif // GUI_H
