#include "gui.h"

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.example.clipboardmanager");

    GuiWindow window;
    return app->run(window);
}
