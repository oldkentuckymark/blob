cmake_minimum_required(VERSION 3.22)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(blob LANGUAGES CXX)

file(GLOB_RECURSE BLOB_DATA_FILES CONFIGURE_DEPENDS "data/*")

set(LICENCES
    LICENSE
)


set(BLOB_HEADERS
    source/util.hpp
    source/ffm.hpp
    source/ffr.hpp
    source/ffp.hpp
    source/meshes.hpp
)



set(BLOB_SOURCES


)

set(BLOB_SDL_SOURCES
    ${BLOB_SOURCES}
    source/platform/sdl/source/main.cpp
)

set(BLOB_GBA_SOURCES
    ${BLOB_SOURCES}
    source/platform/gba/source/main.cpp
)

add_executable(blob-sdl
    ${BLOB_SDL_SOURCES}
    ${BLOB_HEADERS}


)


if(UNIX)
    find_library(SDL2MAIN_LIBRARY SDL2main)
    find_library(SDL2_LIBRARY SDL2)
    find_library(PTHREADS_LIBRARY NAMES pthread)
endif()

target_compile_features(blob-sdl PUBLIC cxx_std_23)
set_target_properties(blob-sdl PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(blob-sdl ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${PTHREAD_LIBRARY})
target_include_directories(blob-sdl PUBLIC source)
target_include_directories(blob-sdl PUBLIC /opt/devkitpro/libgba/include)


add_custom_target(data SOURCES ${BLOB_DATA_FILES})

add_custom_target(blob-gba ALL
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/source/platform/gba -f Makefile
    SOURCES ${BLOB_GBA_SOURCES}
    COMMENT "GBA Build"
    VERBATIM
)
