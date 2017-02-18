add_executable(straylight-test main.cpp
    ../kernel/test/avx.cpp
    ../kernel/test/bind_s7.cpp
    ../kernel/test/cache.cpp
    ../kernel/test/dc.cpp
    ../kernel/test/eval.cpp
    ../kernel/test/heightmap.cpp
    ../kernel/test/mesh.cpp
    ../kernel/test/octree.cpp
    ../kernel/test/region.cpp
    ../kernel/test/solver.cpp
    ../kernel/test/subregion.cpp
    ../kernel/test/tree.cpp
    ../kernel/test/util/shapes.cpp

    ../graph/test/dependencies.cpp
    ../graph/test/interpreter.cpp
    ../graph/test/item.cpp
    ../graph/test/root.cpp
    ../graph/test/tree.cpp
)
target_include_directories(straylight-test PRIVATE ../app/src)
target_link_libraries(straylight-test straylight-kernel straylight-graph)
