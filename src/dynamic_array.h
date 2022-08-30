
struct ArrayChunk {
    ArrayChunk *nextChunk;
    uint8 data[0];
};

struct DynamicArray_Untyped {
    MAllocator *allocator;

    uint32 count;
    uint32 elementsPerChunk;

    uint32 chunkCount;

    ArrayChunk *headChunk;
    ArrayChunk *tailChunk;
};

void DynamicArrayAllocateChunk(DynamicArray_Untyped *array, uint32 elementSize) {
    ArrayChunk *newChunk = (ArrayChunk *)AllocateMem(array->allocator, sizeof(ArrayChunk) + (array->elementsPerChunk * elementSize));
    newChunk->nextChunk = NULL;

    if (array->headChunk == NULL) {
        array->headChunk = newChunk;
        array->tailChunk = newChunk;
    }
    else {
        ArrayChunk *chunk = array->tailChunk;
        chunk->nextChunk = newChunk;
        array->tailChunk = chunk->nextChunk;
    }

    array->chunkCount++;
}

template <typename T>
struct DynamicArray {
    MAllocator *allocator;

    uint32 count;
    uint32 elementsPerChunk;

    uint32 chunkCount;

    ArrayChunk *headChunk;
    ArrayChunk *tailChunk;

    inline T& operator[](const int index) const {
        // (index / elementsPerChunk) to get the chunk
        // then index % chunkCount to get the index

        s32 chunkIndex = index / elementsPerChunk;
        ArrayChunk *chunk = headChunk;
        for (int32 i = 0; i < chunkIndex; i++) {
            chunk = chunk->nextChunk;
        }

        return *(T *)(chunk->data + (sizeof(T) * (index % elementsPerChunk)));
    }
};

template <typename T>
void DynamicArrayAllocateChunk(DynamicArray<T> *array) {
    ArrayChunk *newChunk = (ArrayChunk *)AllocateMem(array->allocator, sizeof(ArrayChunk) + (array->elementsPerChunk * sizeof(T)));
    newChunk->nextChunk = NULL;

    if (array->headChunk == NULL) {
        array->headChunk = newChunk;
        array->tailChunk = newChunk;
    }
    else {
        ArrayChunk *chunk = array->tailChunk;
        chunk->nextChunk = newChunk;
        array->tailChunk = chunk->nextChunk;
    }

     
}

template <typename T>
void DynamicArrayEnsureCapacity(DynamicArray<T> *array, uint32 capacity) {

    ASSERT(array->allocator != NULL);
    ASSERT(array->elementsPerChunk > 0);
    
    if (array->chunkCount * array->elementsPerChunk < capacity) {
        uint32 chunksToAdd = ((capacity / array->elementsPerChunk) - array->chunkCount);
        
        if (array->chunkCount > 0) {
            chunksToAdd++;
        }

        for (int i = 0; i < chunksToAdd; i++) {
            DynamicArrayAllocateChunk(array);
        }
    }
}

template <typename T>
inline DynamicArray<T> MakeDynamicArray(MAllocator *allocator, uint32 elementsPerChunk, uint32 chunkCount = 1) {
    DynamicArray<T> array = {};
    array.allocator = allocator;
    array.elementsPerChunk = elementsPerChunk;

    DynamicArrayEnsureCapacity(&array, chunkCount * elementsPerChunk);
    return array;
}

template <typename T>
inline void DeallocateDynamicArray(DynamicArray<T> *array) {
    ArrayChunk *chunk = array->headChunk;
    ArrayChunk *nextChunk = chunk->nextChunk;
    while (chunk != NULL) {
        DeallocateMem(array->allocator, chunk);

        chunk = nextChunk;

        if (chunk) {
            nextChunk = nextChunk->nextChunk;
        }
    }
}

template <typename T>
inline uint32 PushBack(DynamicArray<T> *array, T elem)  {
    DynamicArrayEnsureCapacity(array, array->count + 1);
    uint32 index = array->count;
    array->count++;
    (*array)[index] = elem;
    return index;
}

template <typename T>
inline T *PushBackPtr(DynamicArray<T> *array)  {
    DynamicArrayEnsureCapacity(array, array->count + 1);
    uint32 index = array->count;
    array->count++;
    T *result = &(*array)[index];
    memset(result, 0, sizeof(T));
    return result;
}

template <typename T>
inline void PushBackUnique(DynamicArray<T> *array, T elem) {
    if (!Contains(array, elem)) {
        PushBack(array, elem);
    }
}

template <typename T>
inline bool PopBack(DynamicArray<T> *array, T *element = NULL) {
    bool result = false;

    if (array->count > 0) {
        if (element != NULL) {
            int32 index = array->count - 1;
            *element = (*array)[index];
        }
        array->count--;
        result = true;
    }

    return result;
}

template <typename T>
inline void DynamicArrayClear(DynamicArray<T> *array) {
    array->count = 0;
}

template <typename T>
inline void DynamicArrayClearToCount(DynamicArray<T> *array, uint32 count) {
    DynamicArrayEnsureCapacity(array, count);

    ArrayChunk *chunk = array->headChunk;
    while (chunk != NULL) {
        memset(chunk->data, 0, array->elementsPerChunk * sizeof(T));
        chunk = chunk->nextChunk;
    }
    
    array->count = count;
}

template <typename T>
inline void ExtendDynamicArray(DynamicArray<T> *array, DynamicArray<T> from) {
    for (int i = 0; i < from.count; i++) {
        PushBack(array, from[i]);
    }
}

template <typename T>
inline void InsertAtIndex(DynamicArray<T> *array, uint32 index, T element) {
    ASSERT(index <= array->count);
    ASSERT(array->elementsPerChunk != 0);

    DynamicArrayEnsureCapacity(array, array->count + 1);

    // @TODO: This can be way more performant by doing MoveMems()

    array->count++;
    for (int i = array->count - 1; i > index; i--) {
        (*array)[i] = (*array)[i - 1];
    }

    (*array)[index] = element;
}

template <typename T>
inline ArrayChunk *GetNthChunk(DynamicArray<T> *array, int32 index) {
    ArrayChunk *chunk = array->headChunk;
    int32 count = 0;
    while (count < index) {
        chunk = chunk->nextChunk;
        count++;

        if (chunk == NULL) {
            return NULL;
        }
    }

    return chunk;
}

template <typename T>
inline void RemoveAtIndex(DynamicArray<T> *array, uint32 index) {
    ASSERT(index < array->count);
    ASSERT(array->elementsPerChunk != 0);
    ASSERT(array->chunkCount > 0);

    if (index == array->count - 1) {
        array->count--;
        return;
    }

    uint32 firstChunkIndex = index / array->elementsPerChunk;
    uint32 lastChunkIndex = Min(array->count / array->elementsPerChunk, array->chunkCount);

    {
        uint32 indexInChunk = index % array->elementsPerChunk;
        uint32 elementsInChunk = firstChunkIndex < lastChunkIndex ?
            array->elementsPerChunk :
            (array->count % array->elementsPerChunk);

        uint32 elementsToMoveInChunk = elementsInChunk - indexInChunk - 1;
        if (elementsToMoveInChunk > 0) {
            ArrayChunk *chunk = GetNthChunk(array, firstChunkIndex);
            memmove(chunk->data + sizeof(T) * indexInChunk,
                    chunk->data + sizeof(T) * (indexInChunk + 1),
                    sizeof(T) * elementsToMoveInChunk);
        }
    }

    // At this point there is a one element "hole" at the end of the
    // first chunk. Fill it in from the first value in the next chunk,
    // then shift the elements in next chunk, which creates a hole at
    // the end of the next chunk, and repeat...
    for (uint32 chunkIndex = firstChunkIndex + 1; chunkIndex <= lastChunkIndex; chunkIndex++) {
        ArrayChunk *prevChunk = GetNthChunk(array, chunkIndex - 1);
        ArrayChunk *chunk = GetNthChunk(array, chunkIndex);

        uint32 elementsInChunk = chunkIndex < lastChunkIndex ?
            array->elementsPerChunk :
            (array->count % array->elementsPerChunk);

        if (elementsInChunk == 0) {
            break;
        }

        memcpy(prevChunk->data + sizeof(T) * (array->elementsPerChunk - 1), chunk->data, sizeof(T));
        memmove(chunk->data, chunk->data + sizeof(T), sizeof(T) * (elementsInChunk - 1));
    }

    array->count--;
}

template <typename T>
inline void RemoveAtIndexBySwap(DynamicArray<T> *array, uint32 index) {
    ASSERT(index < array->count);
    ASSERT(array->elementsPerChunk != 0);

    if (index != array->count - 1) {
        (*array)[index] = (*array)[array->count - 1];
    }

    array->count--;
}

template <typename T>
inline bool Contains(DynamicArray<T> *array, T element, uint32 *index = NULL) {
    bool result = false;

    for (int i = 0; i < array->count; i++) {
        if ((*array)[i] == element) {
            if (index != NULL) { *index = i; }
            result = true;
            break;
        }
    }

    return result;
}

template <typename T>
inline T Last(DynamicArray<T> *array) {
    ASSERT(array->count > 0);

    T result = (*array)[array->count - 1];
    return result;
}

template <typename T>
inline T *LastPtr(DynamicArray<T> *array) {
    ASSERT(array->count > 0);

    T *result = &(*array)[array->count - 1];
    return result;
}

template <typename T>
inline void RemoveLast(DynamicArray<T> *array) {
    ASSERT(array->count > 0);
    RemoveAtIndex(array, array->count - 1);
}

template <typename T>
inline void CopyDynamicArrayIntoBuffer(DynamicArray<T> *array, void *buffer) {

    uint32 chunkSize = sizeof(T) * array->elementsPerChunk;

    ArrayChunk *chunk = array->headChunk;
    while (chunk != NULL) {
        if (chunk != chunk->tailChunk) {
            memcpy(buffer, array->chunks[i]->data, sizeof(T) * array->elementsPerChunk);
        }
        else {
            u32 remainingCount = array->count - ((array->chunkCount - 1) * array->elementsPerChunk);
            memcpy(buffer, chunk->data, sizeof(T) * remainingCount);
        }

        buffer = (uint8 *)buffer + (sizeof(T) * array->elementsPerChunk);
    }
}
