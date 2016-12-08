file(GLOB_RECURSE SRC_FILES
     ../kernel/test/*.cpp
     ../graph2/test/*.cpp
     )

add_executable(straylight-test ${SRC_FILES} main.cpp)
target_link_libraries(straylight-test straylight-kernel straylight-graph)
