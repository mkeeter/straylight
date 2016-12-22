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

add_executable(Straylight MACOSX_BUNDLE
    src/main.cpp
    src/bridge.cpp
    qml.qrc)

target_link_libraries(Straylight
    Qt5::Quick
    Qt5::Gui
    Qt5::OpenGL
    straylight-graph
    ${OPENGL_LIBRARIES})
target_include_directories(Straylight PRIVATE inc)

# Set application Info.plist file
set_target_properties(Straylight PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/app/deploy/mac/Info.plist
)
