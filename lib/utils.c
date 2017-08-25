#include "utils.h"

int ReadVarInt32(uint8_t* p, int32_t* v, uint32_t size)
{
    uint8_t* beg = p;
    int32_t res = 0;
    int more = 1;
    int shift = 0;
    int val;

    while (more)
    {
        if ((p - beg) >= size)
        {
            return WASM_LEB128_OVERFLOW;
        }

        val = (*p) & 0x7f;
        res = res | (val << shift);
        shift += 7;
        more = ((*p++) & 0x80) >> 7;
    }

    res = (res << (64 - shift)) >> (64 - shift);

    *v = res;
    return (int)(p - beg);
}

int ReadVarInt7(uint8_t* p, int8_t* v, uint32_t size)
{
    int32_t res;
    int len;

    len = ReadVarInt32(p, &res, size);
    *v = (int8_t) res;
    return len;
}


int ReadVarUInt32(uint8_t* p, uint32_t* v, uint32_t size)
{
    uint8_t* beg = p;
    uint32_t result = 0;
    int shift = 0;

    while (1)
    {
        if ((p - beg) >= size)
        {
            return WASM_LEB128_OVERFLOW;
        }

        uint8_t byte = *p;
        result |= ((byte & 0x7F) << shift);

        if ((byte & 0x80) == 0)
        {
            break;
        }

        shift += 7;
        p++;
    }

    *v = result;
    return (int)(p - beg) + 1;
}

int ReadVarUInt7(uint8_t* p, uint8_t* v, uint32_t size)
{
    uint32_t res;
    int len;

    len = ReadVarUInt32(p, &res, size);
    *v = (uint8_t) res;
    return len;
}

