
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "libwasm.h"

// The binary structure of WebAssembly files is described in:
//     http://webassembly.org/docs/binary-encoding/
//
// Section types
// Type	        1	Function signature declarations
// Import	    2	Import declarations
// Function	    3	Function declarations
// Table	    4	Indirect function table and other tables
// Memory	    5	Memory attributes
// Global	    6	Global declarations
// Export	    7	Exports
// Start	    8	Start function declaration
// Element	    9	Elements section
// Code	        10	Function bodies (code)
// Data	        11	Data segments

#define REMAINING_SIZE() buffer_size - (uint32_t)(ptr - buffer)

#define SAFE_READ(x) read_length = x; \
\
if (read_length == WASM_LEB128_OVERFLOW) \
{ \
goto parse_error; \
} \
else { \
ptr += read_length; \
} \

#define SAFE_READ_INSTR(x) read_length = x; \
\
if (read_length == WASM_LEB128_OVERFLOW) \
{ \
goto parse_error; \
} \
else { \
disasm->ptr += read_length; \
} \

char* Wasm_SectionNameById(uint8_t id)
{
    switch (id)
    {
        case 0: return "Custom";
        case 1: return "Type";
        case 2: return "Import";
        case 3: return "Function";
        case 4: return "Table";
        case 5: return "Memory";
        case 6: return "Global";
        case 7: return "Export";
        case 8: return "Start";
        case 9: return "Element";
        case 10: return "Code";
        case 11: return "Data";
        default: return "Unknown";
    }
}

char* Wasm_ExternalNameById(uint8_t id)
{
    switch (id)
    {
        case WASM_EXTERNAL_FUNCTION: return "Function";
        case WASM_EXTERNAL_TABLE: return "Table";
        case WASM_EXTERNAL_MEMORY: return "Memory";
        case WASM_EXTERNAL_GLOBAL: return "Global";
        default: return "Unknown";
    }
}

char* Wasm_TypeConstructorById(int id)
{
    switch (id)
    {
        case WASM_TYPE_CONSTRUCTOR_I32: return "i32";
        case WASM_TYPE_CONSTRUCTOR_I64: return "i64";
        case WASM_TYPE_CONSTRUCTOR_F32: return "f32";
        case WASM_TYPE_CONSTRUCTOR_F64: return "f64";
        case WASM_TYPE_CONSTRUCTOR_ANYFUNC: return "anyfunc";
        case WASM_TYPE_CONSTRUCTOR_FUNC: return "func";
        case WASM_TYPE_CONSTRUCTOR_EMPTY_BLOCK: return "empty block_type";
        default: return "Unknown";
    }
}

char* Wasm_OpcodeNameById(WasmOpcode opcode)
{
    switch (opcode)
    {
        case WASM_OPCODE_UNREACHABLE: return "Unreacheable";
        case WASM_OPCODE_NOP: return "nop";
        case WASM_OPCODE_BLOCK: return "block";
        case WASM_OPCODE_LOOP: return "loop";
        case WASM_OPCODE_IF: return "if";
        case WASM_OPCODE_ELSE: return "else";
        case WASM_OPCODE_END: return "end";
        case WASM_OPCODE_BR: return "br";
        case WASM_OPCODE_BR_IF: return "br_if";
        case WASM_OPCODE_BR_TABLE: return "br_table";
        case WASM_OPCODE_RETURN: return "return";
        case WASM_OPCODE_CALL: return "call";
        case WASM_OPCODE_CALL_INDIRECT: return "call_indirect";
        case WASM_OPCODE_DROP: return "drop";
        case WASM_OPCODE_SELECT: return "select";
        case WASM_OPCODE_GET_LOCAL: return "get_local";
        case WASM_OPCODE_SET_LOCAL: return "set_local";
        case WASM_OPCODE_TEE_LOCAL: return "tee_local";
        case WASM_OPCODE_GET_GLOBAL: return "get_global";
        case WASM_OPCODE_SET_GLOBAL: return "set_global";
        case WASM_OPCODE_I32_LOAD: return "i32_load";
        case WASM_OPCODE_I64_LOAD: return "i64_load";
        case WASM_OPCODE_F32_LOAD: return "f32_load";
        case WASM_OPCODE_F64_LOAD: return "f64_load";
        case WASM_OPCODE_I32_LOAD8_S: return "i32_load8_s";
        case WASM_OPCODE_I32_LOAD8_U: return "i32_load8_u";
        case WASM_OPCODE_I32_LOAD16_S: return "i32_load16_s";
        case WASM_OPCODE_I32_LOAD16_U: return "i32_load16_u";
        case WASM_OPCODE_I64_LOAD8_S: return "i64_load8_s";
        case WASM_OPCODE_I64_LOAD8_U: return "i64_load8_u";
        case WASM_OPCODE_I64_LOAD16_S: return "i64_load16_s";
        case WASM_OPCODE_I64_LOAD16_U: return "i64_load16_u";
        case WASM_OPCODE_I64_LOAD32_S: return "i64_load32_s";
        case WASM_OPCODE_I64_LOAD32_U: return "i64_load32_u";
        case WASM_OPCODE_I32_STORE: return "i32_store";
        case WASM_OPCODE_I64_STORE: return "i64_store";
        case WASM_OPCODE_F32_STORE: return "f32_store";
        case WASM_OPCODE_F64_STORE: return "f64_store";
        case WASM_OPCODE_I32_STORE8: return "i32_store8";
        case WASM_OPCODE_I32_STORE16: return "i32_store16";
        case WASM_OPCODE_I64_STORE8: return "i64_store8";
        case WASM_OPCODE_I64_STORE16: return "i64_store16";
        case WASM_OPCODE_I64_STORE32: return "i64_store32";
        case WASM_OPCODE_CURRENT_MEMORY: return "current_memory";
        case WASM_OPCODE_GROW_MEMORY: return "grow_memory";
        case WASM_OPCODE_I32_CONST: return "i32_const";
        case WASM_OPCODE_I64_CONST: return "i64_const";
        case WASM_OPCODE_F32_CONST: return "f32_const";
        case WASM_OPCODE_F64_CONST: return "f64_const";
        default: return "UNKNOWN";
    }

}

int Wasm_ParseGlobalType(uint8_t* buffer, uint32_t buffer_size, WasmGlobalType* global)
{
    uint8_t* ptr = buffer;
    uint8_t mutability;
    int read_length;

    SAFE_READ(ReadVarUInt7(ptr, &global->content_type, REMAINING_SIZE()));
    SAFE_READ(ReadVarUInt7(ptr, &mutability, REMAINING_SIZE()));

    global->mutability = mutability == 1 ? true : false;

    return (int) (ptr - buffer);

parse_error:
    return -1;
}

int Wasm_ParseResizableLimits(uint8_t* buffer, uint32_t buffer_size, WasmResizableLimits* limits)
{
    uint8_t* ptr = buffer;
    int read_length;

    SAFE_READ(ReadVarUInt7(ptr, &limits->flags, REMAINING_SIZE()));
    SAFE_READ(ReadVarUInt32(ptr, &limits->initial, REMAINING_SIZE()));

    if (limits->flags == 1)
    {
        SAFE_READ(ReadVarUInt32(ptr, &limits->maximum, REMAINING_SIZE()));
    }

    return (int) (ptr - buffer);

parse_error:
    return -1;
}

int Wasm_ParseMemoryType(uint8_t* buffer, uint32_t buffer_size, WasmMemoryType* memory)
{
    uint8_t* ptr = buffer;
    int read_length;

    SAFE_READ(Wasm_ParseResizableLimits(buffer, buffer_size, &memory->limits));

    return (int) (ptr - buffer);

parse_error:
    return -1;
}

int Wasm_ParseTableType(uint8_t* buffer, uint32_t buffer_size, WasmTableType* table)
{
    uint8_t* ptr = buffer;
    int read_length;

    SAFE_READ(ReadVarInt7(ptr, &table->elem_type, buffer_size));
    SAFE_READ(Wasm_ParseResizableLimits(ptr, buffer_size, &table->limits));

    return (int) (ptr - buffer);

parse_error:
    return -1;
}

void Wasm_FreeImportEntry(WasmImportEntry* entry)
{
    if (!entry)
    {
        return;
    }

    free(entry->module);
    free(entry->field);
    free(entry);
}

void Wasm_FreeTypeEntry(uint32_t* entry)
{
    free(entry);
}

void Wasm_FreeExportEntry(WasmExportEntry* entry)
{
    if (!entry)
    {
        return;
    }

    free(entry->field);
    free(entry);
}

void Wasm_FreeFunctionEntry(WasmFunctionEntry* entry)
{
    if (!entry)
    {
        return;
    }

    free(entry->locals);
    free(entry);
}

void Wasm_FreeFuncType(WasmFuncType* entry)
{
    if (!entry)
    {
        return;
    }

    free(entry->params);
    free(entry);
}

void Wasm_FreeDataEntry(WasmDataEntry* entry)
{
    if (!entry)
    {
        return;
    }

    free(entry->data);
    free(entry);
}

void Wasm_FreeSection(WasmSection* section)
{
    if (!section)
    {
        return;
    }

    free(section->name);
    ArrayList_Free(section->entries);
    free(section);
}

int Wasm_ParseDataSection(uint8_t* buffer, uint32_t buffer_size, WasmSection* section)
{
    uint8_t* ptr = buffer;
    uint8_t* p;
    uint32_t count, read_length;
    WasmDataEntry* entry;

    ArrayList_Object(section->entries)->fnObjectFree = (OBJECT_FREE_FN) Wasm_FreeDataEntry;
    SAFE_READ(ReadVarUInt32(ptr, &count, REMAINING_SIZE()));

    for (int i = 0; i < count; i++)
    {
        entry = calloc(1, sizeof(WasmDataEntry));

        SAFE_READ(ReadVarUInt32(ptr, &entry->index, REMAINING_SIZE()));
        p = memchr(ptr, WASM_OPCODE_END, REMAINING_SIZE());

        if (!p)
            return -1;

        ptr = p + 1;

        if ((ptr - buffer) > buffer_size)
            goto parse_error;

        SAFE_READ(ReadVarUInt32(ptr, &entry->size, REMAINING_SIZE()));
        entry->data = calloc(1, entry->size);

        memcpy(entry->data, ptr, entry->size);
        ptr += entry->size;

        if ((ptr - buffer) > buffer_size)
            goto parse_error;

        ArrayList_Add(section->entries, entry);
    }

    return (int) (ptr - buffer);

parse_error:
    Wasm_FreeDataEntry(entry);
    return -1;
}

int Wasm_ParseFunctionSection(uint8_t* buffer, uint32_t buffer_size, WasmSection* section)
{
    uint8_t* ptr = buffer;
    uint32_t count, read_length;
    uint32_t* type_index = NULL;

    SAFE_READ(ReadVarUInt32(ptr, &count, REMAINING_SIZE()));

    for (int i = 0; i < count; i++)
    {
        type_index = malloc(sizeof(uint32_t));
        SAFE_READ(ReadVarUInt32(ptr, type_index, REMAINING_SIZE()));

        ArrayList_Add(section->entries, type_index);
    }

    return (int) (ptr - buffer);

parse_error:
    free(type_index);
    return -1;
}

int Wasm_ParseTypeSection(uint8_t* buffer, uint32_t buffer_size, WasmSection* section)
{
    uint8_t* ptr = buffer;
    uint32_t count, read_length;
    int8_t value_type;
    WasmFuncType* entry = NULL;

    SAFE_READ(ReadVarUInt32(ptr, &count, REMAINING_SIZE()));

    for (int i = 0; i < count; i++)
    {
        entry = calloc(1, sizeof(WasmFuncType));
        entry->params = calloc(10, sizeof(WasmValueType)); // WebAssembly limits functions to 10 parameters.

        SAFE_READ(ReadVarInt7(ptr, &entry->form, REMAINING_SIZE()));
        SAFE_READ(ReadVarUInt32(ptr, &entry->param_count, REMAINING_SIZE()));

        if (entry->param_count > 0)
        {

            for (int j = 0; j < entry->param_count; j++)
            {
                SAFE_READ(ReadVarInt7(ptr, &value_type, REMAINING_SIZE()));
                entry->params[j] = (WasmValueType) value_type;
            }
        }

        SAFE_READ(ReadVarUInt7(ptr, &entry->return_count, REMAINING_SIZE()));

        if (entry->return_count > 0)
        {
            SAFE_READ(ReadVarInt7(ptr, &value_type, REMAINING_SIZE()));
            entry->return_type = (WasmValueType) value_type;
        }

        ArrayList_Add(section->entries, entry);
    }

    ArrayList_Object(section->entries)->fnObjectFree = (OBJECT_FREE_FN) Wasm_FreeFuncType;

    return (int) (ptr - buffer);

parse_error:
    Wasm_FreeFuncType(entry);
    return -1;
}

int Wasm_ParseExportSection(uint8_t* buffer, uint32_t buffer_size, WasmSection* section)
{
    uint8_t* ptr = buffer;
    WasmExportEntry* entry = NULL;
    uint32_t count, field_len, read_length;

    ArrayList_Object(section->entries)->fnObjectFree = (OBJECT_FREE_FN) Wasm_FreeExportEntry;
    SAFE_READ(ReadVarUInt32(ptr, &count, REMAINING_SIZE()));

    for (int i = 0; i < count; i++)
    {
        entry = calloc(1, sizeof(WasmExportEntry));

        SAFE_READ(ReadVarUInt32(ptr, &field_len, REMAINING_SIZE()));
        entry->field = calloc(1, field_len + 1);

        if (((ptr + field_len) - buffer) > buffer_size)
            goto parse_error;

        memcpy(entry->field, ptr, field_len);
        ptr += field_len;

        if ((ptr - buffer) >= buffer_size)
            goto parse_error;

        entry->kind = (WasmExternalKind) *ptr++;

        SAFE_READ(ReadVarUInt32(ptr, &entry->index, REMAINING_SIZE()));
        ArrayList_Add(section->entries, entry);
    }

    return (int) (ptr - buffer);

parse_error:
    Wasm_FreeExportEntry(entry);
    return -1;
}

int Wasm_ParseImportSection(uint8_t* buffer, uint32_t buffer_size, WasmSection* section)
{
    uint8_t* ptr = buffer;
    WasmImportEntry* entry = NULL;
    WasmGlobalType global;
    WasmMemoryType memory;
    WasmTableType table;
    uint32_t count, module_len, field_len, function_type, read_length;

    ArrayList_Object(section->entries)->fnObjectFree = (OBJECT_FREE_FN) Wasm_FreeImportEntry;
    SAFE_READ(ReadVarUInt32(ptr, &count, REMAINING_SIZE()));

    for (int i = 0; i < count; i++)
    {
        entry = calloc(1, sizeof(WasmImportEntry));

        SAFE_READ(ReadVarUInt32(ptr, &module_len, REMAINING_SIZE()));
        entry->module = calloc(1, module_len + 1);

        if (((ptr + module_len) - buffer) > buffer_size)
            goto parse_error;

        memcpy(entry->module, ptr, module_len);
        ptr += module_len;

        if ((ptr - buffer) > buffer_size)
            goto parse_error;

        SAFE_READ(ReadVarUInt32(ptr, &field_len, REMAINING_SIZE()));
        entry->field = calloc(1, field_len + 1);

        if (((ptr + field_len) - buffer) > buffer_size)
            goto parse_error;

        memcpy(entry->field, ptr, field_len);
        ptr += field_len;

        if ((ptr - buffer) > buffer_size)
            goto parse_error;

        entry->kind = (WasmExternalKind) *ptr++;

        if ((ptr - buffer) > buffer_size)
            goto parse_error;

        switch (entry->kind)
        {
            case WASM_EXTERNAL_FUNCTION:
                SAFE_READ(ReadVarUInt32(ptr, &function_type, REMAINING_SIZE()));
                break;

            case WASM_EXTERNAL_GLOBAL:
                SAFE_READ(Wasm_ParseGlobalType(ptr, buffer_size, &global));
                break;

            case WASM_EXTERNAL_MEMORY:
                SAFE_READ(Wasm_ParseMemoryType(ptr, buffer_size, &memory));
                break;

            case WASM_EXTERNAL_TABLE:
                SAFE_READ(Wasm_ParseTableType(ptr, buffer_size, &table));
                break;

            default:
                goto parse_error;
        }

        ArrayList_Add(section->entries, entry);
    }

    return (int) (ptr - buffer);

parse_error:
    Wasm_FreeImportEntry(entry);
    return -1;
}

int Wasm_ParseCodeSection(uint8_t* buffer, uint32_t buffer_size, WasmSection* section)
{
    uint8_t* ptr = buffer;
    uint8_t* p_func;
    uint32_t count, read_length;
    uint32_t* type_index;
    int8_t value_type;
    WasmFunctionEntry* entry = NULL;
    WasmLocalEntry* local_entry;

    ArrayList_Object(section->entries)->fnObjectFree = (OBJECT_FREE_FN) Wasm_FreeFunctionEntry;
    SAFE_READ(ReadVarUInt32(ptr, &count, REMAINING_SIZE()));

    for (int i = 0; i < count; i++)
    {
        entry = calloc(1, sizeof(WasmFunctionEntry));

        SAFE_READ(ReadVarUInt32(ptr, &entry->body_size, REMAINING_SIZE()));
        p_func = ptr;
        SAFE_READ(ReadVarUInt32(ptr, &entry->local_count, REMAINING_SIZE()));

        entry->locals = calloc(entry->local_count, sizeof(local_entry));

        for (int j = 0; j < entry->local_count; j++)
        {
            local_entry = &entry->locals[j];
            SAFE_READ(ReadVarUInt32(ptr, &local_entry->count, REMAINING_SIZE()));
            SAFE_READ(ReadVarInt7(ptr, &value_type, REMAINING_SIZE()));
            local_entry->type = (WasmValueType) value_type;
        };

        entry->code = ptr;
        ptr = p_func + entry->body_size;

        if ((ptr - buffer) > buffer_size)
            goto parse_error;

        ArrayList_Add(section->entries, entry);
    }

    return (int) (ptr - buffer);

parse_error:
    Wasm_FreeFunctionEntry(entry);
    return -1;
}

int Wasm_ParseSection(uint8_t* buffer, uint32_t buffer_size, WasmSection** section)
{
    uint8_t* ptr = buffer;
    uint8_t* p2;
    WasmSection* _section;
    uint32_t read_length;

    _section = calloc(1, sizeof(WasmSection));

    if (!_section)
    {
        return -1;
    }

    _section->entries = ArrayList_New();

    SAFE_READ(ReadVarUInt7(ptr, &_section->id, REMAINING_SIZE()));
    SAFE_READ(ReadVarUInt32(ptr, &_section->payload_len, REMAINING_SIZE()));

    if (_section->id == WASM_SECTION_CUSTOM)
    {
        // Custom section have an identity string with the name of the section.
        p2 = ptr;
        p2 += ReadVarUInt32(p2, &_section->name_len, REMAINING_SIZE());
        _section->name = calloc(1, _section->name_len + 1);

        if (((ptr + _section->name_len) - buffer) > buffer_size)
            goto parse_error;

        strcpy(_section->name, (const char*) p2);
    }

    switch (_section->id)
    {
        case WASM_SECTION_IMPORT:
            Wasm_ParseImportSection(ptr, REMAINING_SIZE(), _section);
            break;

        case WASM_SECTION_EXPORT:
            Wasm_ParseExportSection(ptr, REMAINING_SIZE(), _section);
            break;

        case WASM_SECTION_TYPE:
            Wasm_ParseTypeSection(ptr, REMAINING_SIZE(), _section);
            break;

        case WASM_SECTION_DATA:
            Wasm_ParseDataSection(ptr, REMAINING_SIZE(), _section);
            break;

        case WASM_SECTION_FUNCTION:
            Wasm_ParseFunctionSection(ptr, REMAINING_SIZE(), _section);
            break;

        case WASM_SECTION_CODE:
            Wasm_ParseCodeSection(ptr, REMAINING_SIZE(), _section);
            break;

        default:
            break;
    }

    _section->payload_data = ptr;
    ptr += _section->payload_len;

    if ((ptr - buffer) > buffer_size)
        goto parse_error;

    *section = _section;
    return (int) (ptr - buffer);

parse_error:
    Wasm_FreeSection(_section);
    return -1;
}

WasmModule* Wasm_ParseModule(const uint8_t* buffer, uint32_t buffer_size)
{
    WasmModule* module = NULL;
    WasmSection* section = NULL;
    uint8_t* ptr = (uint8_t*) buffer;
    int size;

    if (buffer_size < sizeof(WasmModuleHeader))
    {
        return NULL;
    }

    module = malloc(sizeof(WasmModule));

    if (!module)
    {
        return NULL;
    }

    module->sections = ArrayList_New();
    ArrayList_Object(module->sections)->fnObjectFree = (OBJECT_FREE_FN) Wasm_FreeSection;

    memcpy(module, buffer, sizeof(WasmModuleHeader));
    ptr += sizeof(WasmModuleHeader);

    while (ptr - buffer < buffer_size)
    {
        size = Wasm_ParseSection(ptr, REMAINING_SIZE(), &section);

        if (size < 1)
        {
            break;
        }

        ptr += size;

        ArrayList_Add(module->sections, section);
    }

	return module;

parse_error:
    Wasm_FreeModule(module);
    return NULL;
}

void Wasm_FreeModule(WasmModule* module)
{
    if (!module)
    {
        return;
    }

    ArrayList_Free(module->sections);
    free(module);
}

WasmInstruction* WasmDisasm_NextInstruction(WasmDisasm* disasm)
{
    uint32_t remaining_size, read_length;

    disasm->cur_instruction.opcode = (WasmOpcode) *disasm->ptr;
    disasm->ptr++;
    remaining_size = disasm->size - (uint32_t)(disasm->ptr - disasm->code);

    switch (disasm->cur_instruction.opcode)
    {
        case WASM_OPCODE_END:
            return NULL;

        case WASM_OPCODE_UNREACHABLE:
        case WASM_OPCODE_NOP:
        case WASM_OPCODE_ELSE:
        case WASM_OPCODE_RETURN:
        case WASM_OPCODE_DROP:
        case WASM_OPCODE_SELECT:
            break;

        case WASM_OPCODE_BLOCK:
            SAFE_READ_INSTR(ReadVarInt7(disasm->ptr, &disasm->cur_instruction.p_block.block_type, remaining_size));
            break;
        case WASM_OPCODE_LOOP:
            SAFE_READ_INSTR(ReadVarInt7(disasm->ptr, &disasm->cur_instruction.p_loop.block_type, remaining_size));
            break;
        case WASM_OPCODE_IF:
            SAFE_READ_INSTR(ReadVarInt7(disasm->ptr, &disasm->cur_instruction.p_if.block_type, remaining_size));
            break;

        case WASM_OPCODE_GET_LOCAL:
            SAFE_READ_INSTR(ReadVarUInt32(disasm->ptr, &disasm->cur_instruction.p_get_local.local_index,
                                         remaining_size));
            break;
        case WASM_OPCODE_SET_LOCAL:
            SAFE_READ_INSTR(ReadVarUInt32(disasm->ptr, &disasm->cur_instruction.p_set_local.local_index,
                                          remaining_size));
            break;
        case WASM_OPCODE_TEE_LOCAL:
            SAFE_READ_INSTR(ReadVarUInt32(disasm->ptr, &disasm->cur_instruction.p_tee_local.local_index,
                                          remaining_size));
            break;

        case WASM_OPCODE_GET_GLOBAL:
            SAFE_READ_INSTR(ReadVarUInt32(disasm->ptr, &disasm->cur_instruction.p_get_global.global_index,
                                         remaining_size));
            break;
        case WASM_OPCODE_SET_GLOBAL:
            SAFE_READ_INSTR(ReadVarUInt32(disasm->ptr, &disasm->cur_instruction.p_set_global.global_index,
                                         remaining_size));
            break;

        case WASM_OPCODE_CALL:
            SAFE_READ_INSTR(ReadVarUInt32(disasm->ptr, &disasm->cur_instruction.p_call.function_index, remaining_size));
            break;
        case WASM_OPCODE_CALL_INDIRECT:
            SAFE_READ_INSTR(ReadVarUInt32(disasm->ptr, &disasm->cur_instruction.p_call_indirect.type_index,
                                         remaining_size));
            SAFE_READ_INSTR(ReadVarUInt7(disasm->ptr, &disasm->cur_instruction.p_call_indirect.reserved,
                                         remaining_size));
            break;

        case WASM_OPCODE_I32_CONST:
            SAFE_READ_INSTR(ReadVarInt32(disasm->ptr, &disasm->cur_instruction.p_i32_const.value, remaining_size));
            break;

        default: return NULL;
    }

    return &disasm->cur_instruction;

parse_error:
    return NULL;
}

void Wasm_FreeDisasm(WasmDisasm* disasm)
{
    free(disasm);
}

WasmDisasm* Wasm_Disassemble(uint8_t* code, uint32_t size)
{
    WasmDisasm* disasm;

    disasm = malloc(sizeof(WasmDisasm));
    disasm->code = code;
    disasm->ptr = code;
    disasm->size = size;

    return disasm;

}