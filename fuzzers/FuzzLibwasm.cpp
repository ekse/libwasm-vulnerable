
#include <stddef.h>
#include <stdint.h>

#include "libwasm.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t data_size)
{
    WasmModule* module = Wasm_ParseModule(data, data_size);
    Wasm_FreeModule(module);
    return 0;
}