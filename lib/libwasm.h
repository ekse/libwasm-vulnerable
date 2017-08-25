#ifndef WASM_LIB_LIBWASM_H
#define WASM_LIB_LIBWASM_H

#include <stdint.h>

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

enum wasm_section_type
{
    WASM_SECTION_CUSTOM = 0,
    WASM_SECTION_TYPE,
    WASM_SECTION_IMPORT,
    WASM_SECTION_FUNCTION,
    WASM_SECTION_TABLE,
    WASM_SECTION_MEMORY,
    WASM_SECTION_GLOBAL,
    WASM_SECTION_EXPORT,
    WASM_SECTION_START,
    WASM_SECTION_ELEMENT,
    WASM_SECTION_CODE,
    WASM_SECTION_DATA
};

enum wasm_external_kind
{
    WASM_EXTERNAL_FUNCTION = 0,
    WASM_EXTERNAL_TABLE,
    WASM_EXTERNAL_MEMORY,
    WASM_EXTERNAL_GLOBAL
};

typedef enum wasm_external_kind WasmExternalKind;

enum wasm_opcode
{
    WASM_OPCODE_UNREACHABLE = 0,
    WASM_OPCODE_NOP = 1,
    WASM_OPCODE_BLOCK = 2,
    WASM_OPCODE_LOOP = 3,
    WASM_OPCODE_IF = 4,
    WASM_OPCODE_ELSE = 5,
    WASM_OPCODE_END = 0x0b,
    WASM_OPCODE_BR = 0x0c,
    WASM_OPCODE_BR_IF = 0x0d,
    WASM_OPCODE_BR_TABLE = 0x0e,
    WASM_OPCODE_RETURN = 0x0f,
    WASM_OPCODE_CALL = 0x10,
    WASM_OPCODE_CALL_INDIRECT = 0x11,
    WASM_OPCODE_DROP = 0x1a,
    WASM_OPCODE_SELECT = 0x1b,
    WASM_OPCODE_GET_LOCAL = 0x20,
    WASM_OPCODE_SET_LOCAL = 0x21,
    WASM_OPCODE_TEE_LOCAL = 0x22,
    WASM_OPCODE_GET_GLOBAL = 0x23,
    WASM_OPCODE_SET_GLOBAL = 0x24,
    WASM_OPCODE_I32_LOAD = 0x28,
    WASM_OPCODE_I64_LOAD = 0x29,
    WASM_OPCODE_F32_LOAD = 0x2a,
    WASM_OPCODE_F64_LOAD = 0x2b,
    WASM_OPCODE_I32_LOAD8_S = 0x2c,
    WASM_OPCODE_I32_LOAD8_U = 0x2d,
    WASM_OPCODE_I32_LOAD16_S = 0x2e,
    WASM_OPCODE_I32_LOAD16_U = 0x2f,
    WASM_OPCODE_I64_LOAD8_S = 0x30,
    WASM_OPCODE_I64_LOAD8_U = 0x31,
    WASM_OPCODE_I64_LOAD16_S = 0x32,
    WASM_OPCODE_I64_LOAD16_U = 0x33,
    WASM_OPCODE_I64_LOAD32_S = 0x34,
    WASM_OPCODE_I64_LOAD32_U = 0x35,
    WASM_OPCODE_I32_STORE = 0x36,
    WASM_OPCODE_I64_STORE = 0x37,
    WASM_OPCODE_F32_STORE = 0x38,
    WASM_OPCODE_F64_STORE = 0x39,
    WASM_OPCODE_I32_STORE8 = 0x3a,
    WASM_OPCODE_I32_STORE16 = 0x3b,
    WASM_OPCODE_I64_STORE8 = 0x3c,
    WASM_OPCODE_I64_STORE16 = 0x3d,
    WASM_OPCODE_I64_STORE32 = 0x3e,
    WASM_OPCODE_CURRENT_MEMORY = 0x3f,
    WASM_OPCODE_GROW_MEMORY = 0x40,
    WASM_OPCODE_I32_CONST = 0x41,
    WASM_OPCODE_I64_CONST = 0x42,
    WASM_OPCODE_F32_CONST = 0x43,
    WASM_OPCODE_F64_CONST = 0x44,
    WASM_OPCODE_I32_EQZ	= 0x45,
    WASM_OPCODE_I32_EQ	= 0x46,
    WASM_OPCODE_I32_NE	= 0x47,
    WASM_OPCODE_I32_LT_S = 0x48,
    WASM_OPCODE_I32_LT_U = 0x49,
    WASM_OPCODE_I32_GT_S = 0x4a,
    WASM_OPCODE_I32_GT_U = 0x4b,
    WASM_OPCODE_I32_LE_S = 0x4c,
    WASM_OPCODE_I32_LE_U = 0x4d,
    WASM_OPCODE_I32_GE_S = 0x4e,
    WASM_OPCODE_I32_GE_U = 0x4f,
    WASM_OPCODE_I64_EQZ = 0x50,
    WASM_OPCODE_I64_EQ = 0x51,
    WASM_OPCODE_I64_NE = 0x52,
    WASM_OPCODE_I64_LT_S = 0x53,
    WASM_OPCODE_I64_LT_U = 0x54,
    WASM_OPCODE_I64_GT_S = 0x55,
    WASM_OPCODE_I64_GT_U = 0x56,
    WASM_OPCODE_I64_LE_S = 0x57,
    WASM_OPCODE_I64_LE_U = 0x58,
    WASM_OPCODE_I64_GE_S = 0x59,
    WASM_OPCODE_I64_GE_U = 0x5a,
    WASM_OPCODE_F32_EQ = 0x5b,
    WASM_OPCODE_F32_NE = 0x5c,
    WASM_OPCODE_F32_LT = 0x5d,
    WASM_OPCODE_F32_GT = 0x5e,
    WASM_OPCODE_F32_LE = 0x5f,
    WASM_OPCODE_F32_GE = 0x60,
    WASM_OPCODE_F64_EQ = 0x61,
    WASM_OPCODE_F64_NE = 0x62,
    WASM_OPCODE_F64_LT = 0x63,
    WASM_OPCODE_F64_GT = 0x64,
    WASM_OPCODE_F64_LE = 0x65,
    WASM_OPCODE_F64_GE = 0x66,
    WASM_OPCODE_I32_CLZ = 0x67,
    WASM_OPCODE_I32_CTZ = 0x68,
    WASM_OPCODE_I32_POPCNT = 0x69,
    WASM_OPCODE_I32_ADD = 0x6a,
    WASM_OPCODE_I32_SUB = 0x6b,
    WASM_OPCODE_I32_MUL = 0x6c,
    WASM_OPCODE_I32_DIV_S = 0x6d,
    WASM_OPCODE_I32_DIV_U = 0x6e,
    WASM_OPCODE_I32_REM_S = 0x6f,
    WASM_OPCODE_I32_REM_U = 0x70,
    WASM_OPCODE_I32_AND = 0x71,
    WASM_OPCODE_I32_OR = 0x72,
    WASM_OPCODE_I32_XOR = 0x73,
    WASM_OPCODE_I32_SHL = 0x74,
    WASM_OPCODE_I32_SHR_S = 0x75,
    WASM_OPCODE_I32_SHR_U = 0x76,
    WASM_OPCODE_I32_ROTL = 0x77,
    WASM_OPCODE_I32_ROTR = 0x78,
    WASM_OPCODE_I64_CLZ = 0x79,
    WASM_OPCODE_I64_CTZ = 0x7a,
    WASM_OPCODE_I64_POPCNT = 0x7b,
    WASM_OPCODE_I64_ADD = 0x7c,
    WASM_OPCODE_I64_SUB = 0x7d,
    WASM_OPCODE_I64_MUL = 0x7e,
    WASM_OPCODE_I64_DIV_S = 0x7f,
    WASM_OPCODE_I64_DIV_U = 0x80,
    WASM_OPCODE_I64_REM_S = 0x81,
    WASM_OPCODE_I64_REM_U = 0x82,
    WASM_OPCODE_I64_AND = 0x83,
    WASM_OPCODE_I64_OR = 0x84,
    WASM_OPCODE_I64_XOR = 0x85,
    WASM_OPCODE_I64_SHL = 0x86,
    WASM_OPCODE_I64_SHR_S = 0x87,
    WASM_OPCODE_I64_SHR_U = 0x88,
    WASM_OPCODE_I64_ROTL = 0x89,
    WASM_OPCODE_I64_ROTR = 0x8a,
    WASM_OPCODE_F32_ABS = 0x8b,
    WASM_OPCODE_F32_NEG = 0x8c,
    WASM_OPCODE_F32_CEIL = 0x8d,
    WASM_OPCODE_F32_FLOOR = 0x8e,
    WASM_OPCODE_F32_TRUNC = 0x8f,
    WASM_OPCODE_F32_NEAREST = 0x90,
    WASM_OPCODE_F32_SQRT = 0x91,
    WASM_OPCODE_F32_ADD = 0x92,
    WASM_OPCODE_F32_SUB = 0x93,
    WASM_OPCODE_F32_MUL = 0x94,
    WASM_OPCODE_F32_DIV = 0x95,
    WASM_OPCODE_F32_MIN = 0x96,
    WASM_OPCODE_F32_MAX = 0x97,
    WASM_OPCODE_F32_COPYSIGN = 0x98,
    WASM_OPCODE_F64_ABS = 0x99,
    WASM_OPCODE_F64_NEG = 0x9a,
    WASM_OPCODE_F64_CEIL = 0x9b,
    WASM_OPCODE_F64_FLOOR = 0x9c,
    WASM_OPCODE_F64_TRUNC = 0x9d,
    WASM_OPCODE_F64_NEAREST = 0x9e,
    WASM_OPCODE_F64_SQRT = 0x9f,
    WASM_OPCODE_F64_ADD = 0xa0,
    WASM_OPCODE_F64_SUB = 0xa1,
    WASM_OPCODE_F64_MUL = 0xa2,
    WASM_OPCODE_F64_DIV = 0xa3,
    WASM_OPCODE_F64_MIN = 0xa4,
    WASM_OPCODE_F64_MAX = 0xa5,
    WASM_OPCODE_F64_COPYSIGN = 0xa6,
    WASM_OPCODE_I32_WRAP_I64	= 0xa7,
    WASM_OPCODE_I32_TRUNC_S_F32	= 0xa8,
    WASM_OPCODE_I32_TRUNC_U_F32	= 0xa9,
    WASM_OPCODE_I32_TRUNC_S_F64	= 0xaa,
    WASM_OPCODE_I32_TRUNC_U_F64	= 0xab,
    WASM_OPCODE_I64_EXTEND_S_I32	= 0xac,
    WASM_OPCODE_I64_EXTEND_U_I32	= 0xad,
    WASM_OPCODE_I64_TRUNC_S_F32	= 0xae,
    WASM_OPCODE_I64_TRUNC_U_F32	= 0xaf,
    WASM_OPCODE_I64_TRUNC_S_F64	= 0xb0,
    WASM_OPCODE_I64_TRUNC_U_F64	= 0xb1,
    WASM_OPCODE_F32_CONVERT_S_I32	= 0xb2,
    WASM_OPCODE_F32_CONVERT_U_I32	= 0xb3,
    WASM_OPCODE_F32_CONVERT_S_I64	= 0xb4,
    WASM_OPCODE_F32_CONVERT_U_I64	= 0xb5,
    WASM_OPCODE_F32_DEMOTE_F64	= 0xb6,
    WASM_OPCODE_F64_CONVERT_S_I32	= 0xb7,
    WASM_OPCODE_F64_CONVERT_U_I32	= 0xb8,
    WASM_OPCODE_F64_CONVERT_S_I64	= 0xb9,
    WASM_OPCODE_F64_CONVERT_U_I64	= 0xba,
    WASM_OPCODE_F64_PROMOTE_F32	= 0xbb,
    WASM_OPCODE_I32_REINTERPRET_F32	= 0xbc,
    WASM_OPCODE_I64_REINTERPRET_F64	= 0xbd,
    WASM_OPCODE_F32_REINTERPRET_I32	= 0xbe,
    WASM_OPCODE_F64_REINTERPRET_I64	= 0xbf,
};

typedef enum wasm_opcode WasmOpcode;

enum wasm_value_type
{
    WASM_TYPE_CONSTRUCTOR_I32 = -1,
    WASM_TYPE_CONSTRUCTOR_I64 = -2,
    WASM_TYPE_CONSTRUCTOR_F32 = -3,
    WASM_TYPE_CONSTRUCTOR_F64 = -4,
    WASM_TYPE_CONSTRUCTOR_ANYFUNC = -0x10,
    WASM_TYPE_CONSTRUCTOR_FUNC = -0x20,
    WASM_TYPE_CONSTRUCTOR_EMPTY_BLOCK = -0x40,
};

typedef enum wasm_value_type WasmValueType;

struct wasm_param_blocktype
{
    int8_t block_type;
};

struct wasm_param_local_index
{
    uint32_t local_index;
};

struct wasm_param_global_index
{
    uint32_t global_index;
};

struct wasm_param_call
{
    uint32_t function_index;
};

struct wasm_param_call_indirect
{
    uint32_t type_index;
    uint8_t reserved; // varint1
};

struct wasm_param_i32_const
{
    int32_t value;
};

struct wasm_param_i64_const
{
    int64_t value;
};

struct wasm_param_f32_const
{
    uint32_t value;
};

struct wasm_param_f64_const
{
    uint64_t value;
};

struct wasm_instruction
{
    WasmOpcode opcode;

    union {
        struct wasm_param_blocktype p_block;
        struct wasm_param_blocktype p_loop;
        struct wasm_param_blocktype p_if;
        struct wasm_param_local_index p_get_local;
        struct wasm_param_local_index p_set_local;
        struct wasm_param_local_index p_tee_local;
        struct wasm_param_global_index p_get_global;
        struct wasm_param_global_index p_set_global;
        struct wasm_param_call p_call;
        struct wasm_param_call_indirect p_call_indirect;
        struct wasm_param_i32_const p_i32_const;
        struct wasm_param_f32_const p_f32_const;
    };
};

typedef struct wasm_instruction WasmInstruction;

struct wasm_resizable_limits
{
    uint8_t flags;
    uint32_t initial;
    uint32_t maximum;
};

typedef struct wasm_resizable_limits WasmResizableLimits;

struct wasm_local_entry
{
    uint32_t count;
    WasmValueType type;
};

typedef struct wasm_local_entry WasmLocalEntry;

struct wasm_func_type
{
    int8_t form; // varint7
    uint32_t param_count;
    WasmValueType* params;
    uint8_t return_count;
    WasmValueType return_type;

};

typedef struct wasm_func_type WasmFuncType;

struct wasm_global_type
{
    uint8_t content_type; // varuint7
    bool mutability;
};

typedef struct wasm_global_type WasmGlobalType;

struct wasm_memory_type
{
    WasmResizableLimits limits;
};

typedef struct wasm_memory_type WasmMemoryType;

struct wasm_table_type
{
    int8_t elem_type; // varint7
    WasmResizableLimits limits;
};

typedef struct wasm_table_type WasmTableType;

struct wasm_module_header
{
    uint32_t magic;
    uint32_t version;
};

typedef struct wasm_module_header WasmModuleHeader;

struct wasm_module
{
    WasmModuleHeader hdr;
    ArrayList* sections;
};

struct wasm_data_segment
{
    uint32_t index;

};

typedef struct wasm_module WasmModule;

struct wasm_section
{
    uint8_t id; // varuint7
    uint32_t payload_len;
    uint32_t name_len;
    char* name;
    uint8_t* payload_data;
    ArrayList* entries;
};
typedef struct wasm_section WasmSection;

struct wasm_import_entry
{
    char* module;
    char* field;
    WasmExternalKind kind;
};
typedef struct wasm_import_entry WasmImportEntry;

struct wasm_export_entry
{
    char* field;
    WasmExternalKind kind;
    uint32_t index;
};
typedef struct wasm_export_entry WasmExportEntry;

struct wasm_data_entry
{
    uint32_t index;
    int32_t offset;
    uint32_t size;
    uint8_t* data;
};

typedef struct wasm_data_entry WasmDataEntry;

struct wasm_function_entry
{
    uint32_t body_size;
    uint32_t local_count;
    WasmLocalEntry* locals;
    uint8_t* code;
};

typedef struct wasm_function_entry WasmFunctionEntry;

struct wasm_disasm
{
    uint8_t* code;
    uint32_t size;
    uint8_t* ptr;
    WasmInstruction cur_instruction;
};

typedef struct wasm_disasm WasmDisasm;

char* Wasm_SectionNameById(uint8_t id);
char* Wasm_ExternalNameById(uint8_t id);
char* Wasm_TypeConstructorById(int id);
char* Wasm_OpcodeNameById(WasmOpcode opcode);
WasmModule* Wasm_ParseModule(const uint8_t* buffer, uint32_t buffer_size);
void Wasm_FreeModule(WasmModule* module);
WasmDisasm* Wasm_Disassemble(uint8_t* code, uint32_t size);
void Wasm_FreeDisasm(WasmDisasm* disasm);
WasmInstruction* WasmDisasm_NextInstruction(WasmDisasm* disasm);

#ifdef __cplusplus
}
#endif

#endif /* WASM_LIB_LIBWASM_H */