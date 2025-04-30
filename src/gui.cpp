#include "gui.h"
#include <iostream>

GuiWindow::GuiWindow() {
    set_title("Clipboard Manager");
    set_default_size(400, 600);

    m_scrolledWindow.add(m_listBox);  
    add(m_scrolledWindow); 

    updateClipboardHistory();

    show_all_children();
}

void GuiWindow::startClipboardMonitoring() {
    auto clipboard = Gtk::Clipboard::get();
    clipboard->signal_owner_change().connect(
        [this](GdkEventOwnerChange* /*event*/) {
            updateClipboardHistory();
        }
    );
}

void GuiWindow::updateClipboardHistory() {
    auto clipboard = Gtk::Clipboard::get();
    Glib::ustring clipboardText = clipboard->wait_for_text();

    if (!clipboardText.empty()) {
        if (clipboardHistory.empty() || clipboardHistory.back() != clipboardText) {
            clipboardHistory.push_back(clipboardText);
            addClipboardItem(clipboardText); 
        }
    } else if (clipboardHistory.empty()) {
        if (m_listBox.get_children().empty()) {
            auto label = Gtk::make_managed<Gtk::Label>("No clipboard items yet.");
            label->set_margin_top(10);
            label->set_margin_bottom(10);
            label->set_margin_start(10);
            label->set_margin_end(10);
            label->set_xalign(0.5f);
            m_listBox.append(*label);
            show_all_children();
        }
    }
}



void GuiWindow::addClipboardItem(const Glib::ustring& text) {
    auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);

    auto label = Gtk::make_managed<Gtk::Label>(text);
    label->set_xalign(0.0f);

    auto button = Gtk::make_managed<Gtk::Button>("Copy");
    button->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &GuiWindow::onCopyButtonClicked), text));

    hbox->pack_start(*label, Gtk::PACK_EXPAND_WIDGET);
    hbox->pack_start(*button, Gtk::PACK_SHRINK);

    m_listBox.append(*hbox);

    show_all_children();
}


void GuiWindow::onCopyButtonClicked(const Glib::ustring& text) {
    auto clipboard = Gtk::Clipboard::get();
    clipboard->set_text(text);

    std::cout << "Copied to clipboard: " << text << std::endl;
}

