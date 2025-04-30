cmake_minimum_required(VERSION 3.10)
project(vmcastle)

set(CMAKE_CXX_STANDARD 17)

# Include your "include" folder
include_directories(include)

# Find GTKMM
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

# Define sources
add_executable(vmcastle
    src/main.cpp
    src/gui.cpp
    src/clipboard_manager.cpp
)

target_link_libraries(vmcastle ${GTKMM_LIBRARIES})
