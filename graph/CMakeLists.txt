add_library(straylight-graph STATIC
    src/async.cpp
    src/dependencies.cpp
    src/interpreter.cpp
    src/item.cpp
    src/root.cpp
    src/tree.cpp
    src/command.cpp
    src/translator.cpp
    src/response.cpp

    ../vendor/s7/s7.c)
target_include_directories(straylight-graph SYSTEM PRIVATE)
target_include_directories(straylight-graph PUBLIC include)
