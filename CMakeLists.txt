cmake_minimum_required(VERSION 3.7)
project(libwasm)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-strict-aliasing")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-strict-aliasing")
endif()

include_directories(lib)

if(WASM_FUZZING)
    add_definitions(-DWASM_FUZZING)
    set(LIBFUZZER_LIB_PATH ${CMAKE_SOURCE_DIR}/libFuzzer.a)

    # Clang 4.0 and up
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize-coverage=trace-pc-guard")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-coverage=trace-pc-guard")

    # Clang 3.9
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize-coverage=trace-pc")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-coverage=trace-pc")

    # Clang 3.8
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize-coverage=trace-bb")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-coverage=trace-bb")

    add_subdirectory(fuzzers)
endif()

add_subdirectory(bin)
add_subdirectory(lib)