add_library(sorg STATIC
    sorg.h    
    sorg.cpp
    shader.h
    shader.cpp)

target_link_libraries(sorg PUBLIC glad)
