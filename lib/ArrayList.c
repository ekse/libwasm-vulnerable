/**
 * WinPR: Windows Portable Runtime
 * System.Collections.ArrayList
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#include "utils.h"

/**
 * Gets or sets the number of elements that the ArrayList can contain.
 */

int ArrayList_Capacity(ArrayList* arrayList)
{
    return arrayList->capacity;
}

/**
 * Gets the number of elements actually contained in the ArrayList.
 */

int ArrayList_Count(ArrayList* arrayList)
{
    return arrayList->size;
}

/**
 * Gets the internal list of items contained in the ArrayList.
 */

int ArrayList_Items(ArrayList* arrayList, void** ppItems)
{
    *ppItems = arrayList->array;
    return arrayList->size;
}

/**
 * Gets a value indicating whether the ArrayList has a fixed size.
 */

bool ArrayList_IsFixedSized(ArrayList* arrayList)
{
    return false;
}

/**
 * Gets a value indicating whether the ArrayList is read-only.
 */

bool ArrayList_IsReadOnly(ArrayList* arrayList)
{
    return false;
}

/**
 * Gets the element at the specified index.
 */

void* ArrayList_GetItem(ArrayList* arrayList, int index)
{
    void* obj = NULL;

    if ((index >= 0) && (index < arrayList->size))
    {
        obj = arrayList->array[index];
    }

    return obj;
}

/**
 * Sets the element at the specified index.
 */

void ArrayList_SetItem(ArrayList* arrayList, int index, void* obj)
{
    if ((index >= 0) && (index < arrayList->size))
    {
        arrayList->array[index] = obj;
    }
}

/**
 * Shift a section of the list.
 */

bool ArrayList_Shift(ArrayList* arrayList, int index, int count)
{
    if (count > 0)
    {
        if (arrayList->size + count > arrayList->capacity)
        {
            void** newArray;
            int newCapacity = arrayList->capacity * arrayList->growthFactor;
            newArray = (void**) realloc(arrayList->array, sizeof(void*) * newCapacity);

            if (!newArray)
            {
                return false;
            }

            arrayList->array = newArray;
            arrayList->capacity = newCapacity;
        }

        MoveMemory(&arrayList->array[index + count], &arrayList->array[index],
                   (arrayList->size - index) * sizeof(void*));
        arrayList->size += count;
    }
    else if (count < 0)
    {
        int chunk = arrayList->size - index + count;

        if (chunk > 0)
            MoveMemory(&arrayList->array[index], &arrayList->array[index - count], chunk * sizeof(void*));

        arrayList->size += count;
    }

    return true;
}

/**
 * Removes all elements from the ArrayList.
 */

void ArrayList_Clear(ArrayList* arrayList)
{
    int index;

    for (index = 0; index < arrayList->size; index++)
    {
        if (arrayList->object.fnObjectFree)
        {
            arrayList->object.fnObjectFree(arrayList->array[index]);
        }

        arrayList->array[index] = NULL;
    }

    arrayList->size = 0;
}

/**
 * Determines whether an element is in the ArrayList.
 */

bool ArrayList_Contains(ArrayList* arrayList, void* obj)
{
    int index;
    bool rc = false;

    for (index = 0; index < arrayList->size; index++)
    {
        rc = arrayList->object.fnObjectEquals(arrayList->array[index], obj);

        if (rc)
        {
            break;
        }
    }

    return rc;
}

/**
 * Adds an object to the end of the ArrayList.
 */

int ArrayList_Add(ArrayList* arrayList, void* obj)
{
    int index = -1;

    if (arrayList->size + 1 > arrayList->capacity)
    {
        void** newArray;
        int newCapacity = arrayList->capacity * arrayList->growthFactor;
        newArray = (void**) realloc(arrayList->array, sizeof(void*) * newCapacity);

        if (!newArray)
        {
            goto out;
        }

        arrayList->array = newArray;
        arrayList->capacity = newCapacity;
    }

    arrayList->array[arrayList->size++] = obj;
    index = arrayList->size;

out:
    return index;
}

/*
 * Inserts an element into the ArrayList at the specified index.
 */

bool ArrayList_Insert(ArrayList* arrayList, int index, void* obj)
{
    bool ret = true;

    if ((index >= 0) && (index < arrayList->size))
    {
        if (!ArrayList_Shift(arrayList, index, 1))
        {
            ret = false;
        }
        else
        {
            arrayList->array[index] = obj;
        }
    }

    return ret;
}

/**
 * Removes the first occurrence of a specific object from the ArrayList.
 */

bool ArrayList_Remove(ArrayList* arrayList, void* obj)
{
    int index;
    bool found = false;
    bool ret = true;

    for (index = 0; index < arrayList->size; index++)
    {
        if (arrayList->array[index] == obj)
        {
            found = true;
            break;
        }
    }

    if (found)
    {
        if (arrayList->object.fnObjectFree)
        {
            arrayList->object.fnObjectFree(arrayList->array[index]);
        }

        ret = ArrayList_Shift(arrayList, index, -1);
    }

    return ret;
}

/**
 * Removes the element at the specified index of the ArrayList.
 */

bool ArrayList_RemoveAt(ArrayList* arrayList, int index)
{
    bool ret = true;

    if ((index >= 0) && (index < arrayList->size))
    {
        if (arrayList->object.fnObjectFree)
        {
            arrayList->object.fnObjectFree(arrayList->array[index]);
        }

        ret = ArrayList_Shift(arrayList, index, -1);
    }

    return ret;
}

/**
 * Searches for the specified Object and returns the zero-based index of the first occurrence within the entire
 * ArrayList.
 */

int ArrayList_IndexOf(ArrayList* arrayList, void* obj, int startIndex, int count)
{
    int index;
    bool found = false;

    if (startIndex < 0)
    {
        startIndex = 0;
    }

    if (count < 0)
    {
        count = arrayList->size;
    }

    for (index = startIndex; index < startIndex + count; index++)
    {
        if (arrayList->object.fnObjectEquals(arrayList->array[index], obj))
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        index = -1;
    }

    return index;
}

/**
 * Searches for the specified Object and returns the zero-based index of the last occurrence within the entire
 * ArrayList.
 */

int ArrayList_LastIndexOf(ArrayList* arrayList, void* obj, int startIndex, int count)
{
    int index;
    bool found = false;

    if (startIndex < 0)
    {
        startIndex = 0;
    }

    if (count < 0)
    {
        count = arrayList->size;
    }

    for (index = startIndex + count - 1; index >= startIndex; index--)
    {
        if (arrayList->object.fnObjectEquals(arrayList->array[index], obj))
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        index = -1;
    }

    return index;
}

static bool ArrayList_DefaultCompare(void* objA, void* objB)
{
    return objA == objB ? true : false;
}

ArrayList* ArrayList_New()
{
    ArrayList* arrayList = NULL;
    arrayList = (ArrayList*) calloc(1, sizeof(ArrayList));

    if (!arrayList)
    {
        return NULL;
    }

    arrayList->capacity = 32;
    arrayList->growthFactor = 2;
    arrayList->object.fnObjectEquals = ArrayList_DefaultCompare;
    arrayList->array = (void**) malloc(arrayList->capacity * sizeof(void*));

    if (!arrayList->array)
    {
        goto out_free;
    }

    return arrayList;

out_free:
    free(arrayList);
    return NULL;
}

void ArrayList_Free(ArrayList* arrayList)
{
    if (!arrayList)
    {
        return;
    }

    ArrayList_Clear(arrayList);
    free(arrayList->array);
    free(arrayList);
}