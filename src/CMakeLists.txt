cmake_minimum_required (VERSION 3.9)

project (libtcod C CXX)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_library(TCOD)
add_library(TCOD::TCOD ALIAS TCOD)
target_compile_features(TCOD PUBLIC cxx_std_14)
target_link_libraries(TCOD PUBLIC CONAN_PKG::sdl2)
target_link_libraries(TCOD PRIVATE CONAN_PKG::zlib)

target_include_directories(TCOD PUBLIC ./)
target_include_directories(TCOD PRIVATE ./vendor)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(TCOD PRIVATE LIBTCOD_EXPORTS)
else()
    target_compile_definitions(TCOD PUBLIC LIBTCOD_STATIC)
endif()

if(MSVC)
    target_compile_definitions(TCOD PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(TCOD PRIVATE TCOD_IGNORE_DEPRECATED)

include(sources.cmake)

if(IPO_SUPPORTED)
    set_property(TARGET TCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_property(TARGET TCOD PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
endif()

if(MSVC)
    set_property(TARGET TCOD PROPERTY OUTPUT_NAME libtcod)
else()
    set_property(TARGET TCOD PROPERTY OUTPUT_NAME tcod)
endif()

if(MSVC)
  target_compile_options(TCOD PRIVATE /W4)
else()
  target_compile_options(TCOD PRIVATE -Wall -Wextra)
endif()
