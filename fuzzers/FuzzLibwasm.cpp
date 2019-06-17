
#include <stddef.h>
#include <stdint.h>

#include "libwasm.h"

void Fuzz_WasmDisassemble(const uint8_t* data, size_t data_size)
{
    WasmDisasm* disasm;
    WasmInstruction* instr;

    disasm = Wasm_Disassemble((uint8_t*) data, data_size);

    while ((instr = WasmDisasm_NextInstruction(disasm)))
    {
        Wasm_OpcodeNameById(instr->opcode);
    }

    Wasm_FreeDisasm(disasm);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t data_size)
{
    Fuzz_WasmDisassemble(data, data_size);
    
    return 0;
}