add_library(straylight-graph STATIC
    src/dependencies.cpp
    src/symbol.cpp
    src/key.cpp

    ../vendor/s7/s7.c)
target_include_directories(straylight-graph SYSTEM PRIVATE)
target_include_directories(straylight-graph PUBLIC include)

