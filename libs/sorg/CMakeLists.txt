add_library(sorg STATIC
    sorg.h    
    sorg.cpp)

target_link_libraries(sorg PUBLIC glad)
