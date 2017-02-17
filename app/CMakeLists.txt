# Instruct CMake to run moc, uic, and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Make the build directory an include target (for generated moc_ files)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MACOSX_RPATH ON)

find_package(OpenGL REQUIRED)

find_package(Qt5Core REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Concurrent REQUIRED)

add_executable(Straylight MACOSX_BUNDLE
    src/main.cpp
    src/core/bridge.cpp
    src/core/undo.cpp
    src/ui/material.cpp
    src/ui/syntax.cpp
    src/render/canvas.cpp
    src/render/axes.cpp
    src/render/picker.cpp
    src/render/drag.cpp
    src/render/handle.cpp
    src/render/point_handle.cpp
    src/render/renderer.cpp
    src/render/blitter.cpp
    src/bind/bind_s7.cpp
    resources.qrc)

target_link_libraries(Straylight
    Qt5::Quick
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Concurrent
    straylight-graph
    straylight-kernel
    ${OPENGL_LIBRARIES})
target_include_directories(Straylight PRIVATE src)

# Set application Info.plist file
set_target_properties(Straylight PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/app/deploy/mac/Info.plist
)
