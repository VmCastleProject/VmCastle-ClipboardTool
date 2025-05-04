cmake_minimum_required(VERSION 3.10)
project(clipboard_manager VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(GLIB REQUIRED glib-2.0)

# Check for xclip (required for clipboard operations on X11)
find_program(XCLIP_EXECUTABLE xclip)
if(NOT XCLIP_EXECUTABLE)
    message(WARNING "xclip not found. Please install xclip for clipboard functionality.")
endif()

# Include directories
include_directories(
    ${GTK4_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
)

# Link directories
link_directories(
    ${GTK4_LIBRARY_DIRS}
    ${GLIB_LIBRARY_DIRS}
)

# Add compile options
add_compile_options(
    ${GTK4_CFLAGS_OTHER}
    ${GLIB_CFLAGS_OTHER}
)

# Set source files
set(SOURCES
    src/main.cpp
    src/clipboard_manager.cpp
    src/clipboard_entry.cpp
    src/ui/main_window.cpp
    src/ui/shortcuts.cpp
)

# Add resources
configure_file(resources/app_icon.svg ${CMAKE_BINARY_DIR}/resources/app_icon.svg COPYONLY)
configure_file(resources/tray_icon.svg ${CMAKE_BINARY_DIR}/resources/tray_icon.svg COPYONLY)

# Create executable
add_executable(clipboard_manager ${SOURCES})

# Link libraries
target_link_libraries(clipboard_manager
    ${GTK4_LIBRARIES}
    ${GLIB_LIBRARIES}
)

# Install
install(TARGETS clipboard_manager DESTINATION bin)
install(FILES 
    resources/app_icon.svg
    resources/tray_icon.svg
    DESTINATION share/clipboard_manager/resources)

# Set installation path for binary
set(CMAKE_INSTALL_PREFIX /usr/local)
