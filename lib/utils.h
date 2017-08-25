#ifndef LIBWASM_UTILS_H
#define LIBWASM_UTILS_H

#include <stdbool.h>
#include <stdint.h>

#define CopyMemory(Destination, Source, Length)		memcpy((Destination), (Source), (Length))
#define MoveMemory(Destination, Source, Length)		memmove((Destination), (Source), (Length))

#define WASM_LEB128_OVERFLOW -1

int ReadVarInt7(uint8_t* p, int8_t* v, uint32_t size);
int ReadVarInt32(uint8_t* p, int32_t* v, uint32_t size);
int ReadVarUInt7(uint8_t* p, uint8_t* v, uint32_t size);
int ReadVarUInt32(uint8_t* p, uint32_t* v, uint32_t size);

typedef void* (*OBJECT_NEW_FN)(void);
typedef void (*OBJECT_INIT_FN)(void* obj);
typedef void (*OBJECT_UNINIT_FN)(void* obj);
typedef void (*OBJECT_FREE_FN)(void* obj);
typedef bool (*OBJECT_EQUALS_FN)(void* objA, void* objB);

struct _wObject
{
    OBJECT_NEW_FN fnObjectNew;
    OBJECT_INIT_FN fnObjectInit;
    OBJECT_UNINIT_FN fnObjectUninit;
    OBJECT_FREE_FN fnObjectFree;
    OBJECT_EQUALS_FN fnObjectEquals;
};
typedef struct _wObject wObject;

struct _wArrayList
{
    int capacity;
    int growthFactor;

    int size;
    void** array;

    wObject object;
};
typedef struct _wArrayList ArrayList;

#define ArrayList_Object(_arrayList)	(&_arrayList->object)

int ArrayList_Capacity(ArrayList* arrayList);
int ArrayList_Count(ArrayList* arrayList);
int ArrayList_Items(ArrayList* arrayList, void** ppItems);
bool ArrayList_IsFixedSized(ArrayList* arrayList);
bool ArrayList_IsReadOnly(ArrayList* arrayList);
bool ArrayList_IsSynchronized(ArrayList* arrayList);
void ArrayList_Lock(ArrayList* arrayList);
void ArrayList_Unlock(ArrayList* arrayList);
void* ArrayList_GetItem(ArrayList* arrayList, int index);
void ArrayList_SetItem(ArrayList* arrayList, int index, void* obj);
void ArrayList_Clear(ArrayList* arrayList);
bool ArrayList_Contains(ArrayList* arrayList, void* obj);
int ArrayList_Add(ArrayList* arrayList, void* obj);
bool ArrayList_Insert(ArrayList* arrayList, int index, void* obj);
bool ArrayList_Remove(ArrayList* arrayList, void* obj);
bool ArrayList_RemoveAt(ArrayList* arrayList, int index);
int ArrayList_IndexOf(ArrayList* arrayList, void* obj, int startIndex, int count);
int ArrayList_LastIndexOf(ArrayList* arrayList, void* obj, int startIndex, int count);
ArrayList* ArrayList_New();
void ArrayList_Free(ArrayList* arrayList);

#endif /* LIBWASM_UTILS_H */
