set(ZLIB_INCLUDE_DIR "C:/msys64/mingw64/include")
set(ZLIB_LIBRARY "C:/msys64/mingw64/lib/libz.a")
find_package(ZLIB REQUIRED)

add_library(iorg STATIC iorg.cpp)

include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(iorg PRIVATE ZLIB::ZLIB)
target_include_directories(iorg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ZLIB_INCLUDE_DIRS})
