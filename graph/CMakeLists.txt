file(GLOB SRC_FILES src/*.cpp)
add_library(straylight-graph STATIC ${SRC_FILES} vendor/s7/s7.c)
target_include_directories(straylight-graph SYSTEM PRIVATE vendor)
target_include_directories(straylight-graph PUBLIC include)
