
struct ArrayChunk {
    ArrayChunk *nextChunk;
    uint8 data[0];
};

struct ChunkedArray_Untyped {
    MAllocator *allocator;

    uint32 count;
    uint32 elementsPerChunk;

    uint32 chunkCount;

    ArrayChunk *headChunk;
    ArrayChunk *tailChunk;
};

void ChunkedArrayAllocateChunk(ChunkedArray_Untyped *array, uint32 elementSize) {
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
struct ChunkedArray {
    MAllocator *allocator;

    uint32 count;
    uint32 elementsPerChunk;

    uint32 chunkCount;

    ArrayChunk *headChunk;
    ArrayChunk *tailChunk;

    inline T& operator[](const int index) const {
        // (index / chunkCount) to get the chunk
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
void ChunkedArrayAllocateChunk(ChunkedArray<T> *array) {
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

    array->chunkCount++;
}

template <typename T>
void ChunkedArrayEnsureCapacity(ChunkedArray<T> *array, uint32 capacity) {

    ASSERT(array->allocator != NULL);
    ASSERT(array->elementsPerChunk > 0);
    
    if (array->chunkCount * array->elementsPerChunk < capacity) {
        uint32 chunksToAdd = ((capacity / array->elementsPerChunk) - array->chunkCount);
        
        if (array->chunkCount > 0) {
            chunksToAdd++;
        }

        for (int i = 0; i < chunksToAdd; i++) {
            ChunkedArrayAllocateChunk(array);
        }
    }
}

template <typename T>
inline ChunkedArray<T> MakeChunkedArray(MAllocator *allocator, uint32 elementsPerChunk, uint32 chunkCount = 1) {
    ChunkedArray<T> array = {};
    array.allocator = allocator;
    array.elementsPerChunk = elementsPerChunk;

    ChunkedArrayEnsureCapacity(&array, chunkCount * elementsPerChunk);
    return array;
}

template <typename T>
inline void DeallocateChunkedArray(ChunkedArray<T> *array) {
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
inline uint32 PushBack(ChunkedArray<T> *array, T elem)  {
    ChunkedArrayEnsureCapacity(array, array->count + 1);
    uint32 index = array->count;
    array->count++;
    (*array)[index] = elem;
    return index;
}

template <typename T>
inline T *PushBackPtr(ChunkedArray<T> *array)  {
    ChunkedArrayEnsureCapacity(array, array->count + 1);
    uint32 index = array->count;
    array->count++;
    T *result = &(*array)[index];
    Plat.ClearMem(result, sizeof(T));
    return result;
}

template <typename T>
inline void PushBackUnique(ChunkedArray<T> *array, T elem) {
    if (!Contains(array, elem)) {
        PushBack(array, elem);
    }
}

template <typename T>
inline bool PopBack(ChunkedArray<T> *array, T *element = NULL) {
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
inline void ChunkedArrayClear(ChunkedArray<T> *array) {
    array->count = 0;
}

template <typename T>
inline void ChunkedArrayClearToCount(ChunkedArray<T> *array, uint32 count) {
    ChunkedArrayEnsureCapacity(array, count);

    ArrayChunk *chunk = array->headChunk;
    while (chunk != NULL) {
        Plat.ClearMem(chunk->data, array->elementsPerChunk * sizeof(T));
        chunk = chunk->nextChunk;
    }
    
    array->count = count;
};

template <typename T>
inline void ExtendChunkedArray(ChunkedArray<T> *array, ChunkedArray<T> from) {
    for (int i = 0; i < from.count; i++) {
        PushBack(array, from[i]);
    }
}

template <typename T>
inline void InsertAtIndex(ChunkedArray<T> *array, uint32 index, T element) {
    ASSERT(index <= array->count);
    ASSERT(array->elementsPerChunk != 0);

    ChunkedArrayEnsureCapacity(array, array->count + 1);

    // @TODO: This can be way more performant by doing MoveMems()

    array->count++;
    for (int i = array->count - 1; i > index; i--) {
        (*array)[i] = (*array)[i - 1];
    }

    (*array)[index] = element;
}

template <typename T>
inline ArrayChunk *GetNthChunk(ChunkedArray<T> *array, int32 index) {
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
inline void RemoveAtIndex(ChunkedArray<T> *array, uint32 index) {
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
            Plat.MoveMem(chunk->data + sizeof(T) * indexInChunk,
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

        Plat.CopyMem(prevChunk->data + sizeof(T) * (array->elementsPerChunk - 1), chunk->data, sizeof(T));
        Plat.MoveMem(chunk->data, chunk->data + sizeof(T), sizeof(T) * (elementsInChunk - 1));
    }

    array->count--;
}

template <typename T>
inline void RemoveAtIndexBySwap(ChunkedArray<T> *array, uint32 index) {
    ASSERT(index < array->count);
    ASSERT(array->elementsPerChunk != 0);

    if (index != array->count - 1) {
        (*array)[index] = (*array)[array->count - 1];
    }

    array->count--;
}

template <typename T>
inline bool Contains(ChunkedArray<T> *array, T element, uint32 *index = NULL) {
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
inline T Last(ChunkedArray<T> *array) {
    ASSERT(array->count > 0);

    T result = (*array)[array->count - 1];
    return result;
}

template <typename T>
inline T *LastPtr(ChunkedArray<T> *array) {
    ASSERT(array->count > 0);

    T *result = &(*array)[array->count - 1];
    return result;
}

template <typename T>
inline void RemoveLast(ChunkedArray<T> *array) {
    ASSERT(array->count > 0);
    RemoveAtIndex(array, array->count - 1);
}

template <typename T>
inline void CopyChunkedArrayIntoBuffer(ChunkedArray<T> *array, void *buffer) {

    uint32 chunkSize = sizeof(T) * array->elementsPerChunk;

    ArrayChunk *chunk = array->headChunk;
    while (chunk != NULL) {
        if (chunk != chunk->tailChunk) {
            Plat.CopyMem(buffer, array->chunks[i]->data, sizeof(T) * array->elementsPerChunk);
        }
        else {
            u32 remainingCount = array->count - ((array->chunkCount - 1) * array->elementsPerChunk);
            Plat.CopyMem(buffer, chunk->data, sizeof(T) * remainingCount);
        }

        buffer = (uint8 *)buffer + (sizeof(T) * array->elementsPerChunk);
    }
}
