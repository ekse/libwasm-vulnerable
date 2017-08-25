
#include <stdio.h>
#include <stdlib.h>

#include "libwasm.h"

int ReadFile(const char* filename, char** buffer)
{
    char* data = NULL;

    FILE* fp = fopen(filename, "rb");

    if (!fp)
    {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    data = malloc(size);
    fread(data, 1, size, fp);

    *buffer = data;

    return size;
}

int main(int argc, char* argv[])
{
    WasmSection* section;

    if (argc != 2)
    {
        printf("usage: wasm-dump <file.wasm>\n");
        return 0;
    }

    char* data;
    int file_size = ReadFile(argv[1], &data);

    if (file_size < 0)
    {
        printf("Failed to read %s\n", argv[1]);
        return -1;
    }

    printf("Read %d bytes from %s.\n", file_size, argv[1]);

    WasmModule* module = Wasm_ParseModule((uint8_t*) data, (uint32_t) file_size);

    if (!module)
    {
        printf("Invalid wasm file.\n");
        return(0);
    }

    printf("version: %d\n\n", module->hdr.version);
    printf("Sections:\n");

    for(int i = 0; i < ArrayList_Count(module->sections); i++)
    {
        section = ArrayList_GetItem(module->sections, i);
        printf("id: %u (%s)\n", section->id, Wasm_SectionNameById(section->id));
        printf("payload_len: %u\n", section->payload_len);

        switch (section->id)
        {
            case WASM_SECTION_CUSTOM:
                printf("section name: %s\n", section->name);
                break;

            case WASM_SECTION_DATA:
                printf("data entries: %d\n", ArrayList_Count(section->entries));
                break;

            case WASM_SECTION_IMPORT:
                printf("import entries: %d\n", ArrayList_Count(section->entries));

                for (int j = 0; j < ArrayList_Count(section->entries); j++)
                {
                    WasmImportEntry* entry = ArrayList_GetItem(section->entries, j);

                    printf("module: %s field: %s kind: %s\n", entry->module, entry->field,
                           Wasm_ExternalNameById(entry->kind));
                }

                break;

            case WASM_SECTION_EXPORT:
                printf("export entries: %d\n", ArrayList_Count(section->entries));

                for (int j = 0; j < ArrayList_Count(section->entries); j++)
                {
                    WasmExportEntry* entry = ArrayList_GetItem(section->entries, j);

                    printf("field: %s kind: %s\n", entry->field, Wasm_ExternalNameById(entry->kind));
                }

                break;

            case WASM_SECTION_TYPE:
                printf("type entries: %d\n", ArrayList_Count(section->entries));

                for (int j = 0; j < ArrayList_Count(section->entries); j++)
                {
                    WasmFuncType* entry = ArrayList_GetItem(section->entries, j);

                    printf("type constructor: %s param count: %d\n", Wasm_TypeConstructorById(entry->form),
                           entry->param_count);
                }

                break;

            case WASM_SECTION_FUNCTION:
                printf("function entries: %d\n", ArrayList_Count(section->entries));
                break;

            case WASM_SECTION_CODE:
                for (int j = 0; j < ArrayList_Count(section->entries); j++)
                {
                    WasmDisasm* disasm;
                    WasmInstruction* instr;
                    WasmFunctionEntry* entry = ArrayList_GetItem(section->entries, j);

                    printf("\nfunction #%d length: %d\n", j, entry->body_size);

                    disasm = Wasm_Disassemble(entry->code, entry->body_size);

                    while ((instr = WasmDisasm_NextInstruction(disasm)))
                    {
                        printf("%s\n", Wasm_OpcodeNameById(instr->opcode));
                    }

                    Wasm_FreeDisasm(disasm);
                }

                break;

            default:
                break;
        }

        printf("\n");
    }

    Wasm_FreeModule(module);
    free(data);

    return 0;
}