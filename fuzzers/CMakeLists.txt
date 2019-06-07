set(FUZZ_LIBWASM_SOURCES FuzzLibwasm.cpp)

add_executable(FuzzLibwasm ${FUZZ_LIBWASM_SOURCES})

target_link_libraries(FuzzLibwasm ${CMAKE_THREAD_LIBS_INIT} libwasm)
