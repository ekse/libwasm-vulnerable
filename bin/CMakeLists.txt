
set(WASMDUMP_SOURCE_FILES wasm-dump.c)

add_executable(wasm-dump ${WASMDUMP_SOURCE_FILES})
target_link_libraries(wasm-dump libwasm)