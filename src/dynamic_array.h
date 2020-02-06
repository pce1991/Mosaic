
struct DynamicArray_Untyped {
    uint32 count;
    uint32 capacity;
    void *data;
};

template <typename T>
struct DynamicArray {
    uint32 count;
    uint32 capacity;

    T *data;

    // Compiler complains if this isn't a non-static member function!
    inline T& operator[](const int index) const {
        return data[index];
    }
};


template <typename T>
DynamicArray<T> MakeDynamicArray(uint32 capacity = 0) {
    DynamicArray<T> array = {};
    array.count = 0;
    array.capacity = capacity;

    if (capacity > 0) {
        array.data = (T *)malloc(sizeof(T) * capacity);
    }
    
    return array;
}

template <typename T>
void DeallocateDynamicArray(DynamicArray<T> *array) {
    if (array->capacity > 0) {
        free(array->data);
    }
    array->count = 0;
    array->capacity = 0;
    array->data = NULL;
}

template <typename T>
DynamicArray<T> CopyDynamicArray(DynamicArray<T> *array) {
    DynamicArray<T> result = MakeDynamicArray<T>(array->capacity);

    result.count = array->count;
    memcpy(result.data, array->data, sizeof(T) * array->count);

    return result;
}

inline void DynamicArrayEnsureCapacity(DynamicArray_Untyped *array, uint32 capacity, uint32 elementSize) {
    if (array->capacity < capacity) {
        uint32 nextCapacity = array->capacity;
        if (nextCapacity == 0) {
            nextCapacity = 1;
        }

        while (capacity > nextCapacity) {
            nextCapacity *= 2;
        }

        if (array->capacity == 0) {
            array->data = malloc(elementSize * nextCapacity);
        }
        else {
            array->data = realloc(array->data, elementSize * nextCapacity);
        }
        array->capacity = nextCapacity;
    }
}

template <typename T>
inline void DynamicArrayEnsureCapacity(DynamicArray<T> *array, uint32 capacity) {
    DynamicArrayEnsureCapacity((DynamicArray_Untyped *)array, capacity, sizeof(T));
}

inline void DynamicArrayEnsureCapacity(uint32 *arrayCapacity, uint32 arrayCount, void **data, uint32 elemSize, uint32 capacity) {
    if (*arrayCapacity < capacity) {
        uint32 nextCapacity = *arrayCapacity;
        if (nextCapacity == 0) {
            nextCapacity = 1;
        }

        while (capacity > nextCapacity) {
            nextCapacity *= 2;
        }

        if (*arrayCapacity == 0) {
            *data = malloc(elemSize * nextCapacity);
        }
        else {
            *data = realloc(*data, elemSize * nextCapacity);
        }
        *arrayCapacity = nextCapacity;
    }
}


template <typename T>
void OverwriteDynamicArray(DynamicArray<T> *array, void *ptr, uint32 count) {
    if (array->capacity < count) {
        DynamicArrayEnsureCapacity(array, count + 1);
    }
    
    array->count = count;
    memcpy(array->data, ptr, sizeof(T) * count);
}

template <typename T>
void OverwriteDynamicArray(DynamicArray<T> *array, DynamicArray<T> *from) {
    OverwriteDynamicArray(array, from->data, from->count);
}

template <typename T>
void ExtendDynamicArray(DynamicArray<T> *array, void *ptr, uint32 count) {
    if (array->capacity < array->count + count) {
        DynamicArrayEnsureCapacity(array, Max(array->count + count, 1u));
    }

    memcpy(array->data + array->count, ptr, sizeof(T) * count);
    array->count += count;
}

template <typename T>
void ExtendDynamicArray(DynamicArray<T> *array, DynamicArray<T> *from) {
    ExtendDynamicArray(array, from->data, from->count);
}

template <typename T>
inline void DynamicArrayClear(DynamicArray<T> *array) {
    array->count = 0;
}

template <typename T>
inline int PushBack(DynamicArray<T> *array, T element) {
    DynamicArrayEnsureCapacity(array, array->count + 1);
    uint32 index = array->count;
    array->data[index] = element;
    array->count++;

    return index;
}

inline int PushBack(uint32 *arrayCapacity, uint32 *arrayCount, void **arrayData, uint32 elemSize, void *elem) {
    DynamicArrayEnsureCapacity(arrayCapacity, *arrayCount, arrayData, elemSize, *arrayCount + 1);
    uint32 index = *arrayCount;
    memcpy((void *)((uint8 *)*arrayData + (index * elemSize)), elem, elemSize);
    *arrayCount += 1;

    return index;
}

template <typename T>
inline T *PushBackPtr(DynamicArray<T> *array) {
    DynamicArrayEnsureCapacity(array, array->count + 1);
    uint32 index = array->count;
    array->count++;

    T *result = array->data + index;
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
            *element = array->data[array->count - 1];
        }
        array->count--;
        result = true;
    }

    return result;
}

template <typename T>
inline bool PopFront(DynamicArray<T> *array, T *element = NULL) {
    bool result = false;

    if (array->count > 0) {
        if (element != NULL) {
            *element = array->data[0];
        }
        RemoveAtIndex(array, 0);

        result = true;
    }

    return result;
}

inline void *InsertPtrAtIndex(DynamicArray_Untyped *array, uint32 index, uint32 elementSize) {
    ASSERT(index <= array->count);

    DynamicArrayEnsureCapacity(array, array->count + 1, elementSize);

    uint32 elementsToMove = array->count - index;
    if (elementsToMove > 0) {
        memmove(BUFFER_OFFSET(array->data, elementSize * (index + 1)),
                BUFFER_OFFSET(array->data, elementSize * index),
                elementSize * elementsToMove);
    }

    void *dest = BUFFER_OFFSET(array->data, elementSize * index);
    memset(dest, 0, elementSize);
    array->count++;

    return dest;
}

template <typename T>
inline void InsertAtIndex(DynamicArray<T> *array, uint32 index, T element) {
    ASSERT(index <= array->count);

    DynamicArrayEnsureCapacity(array, array->count + 1);

    uint32 elementsToMove = array->count - index;
    if (elementsToMove > 0) {
        memmove(array->data + index + 1, array->data + index, sizeof(T) * elementsToMove);
    }

    array->data[index] = element;
    array->count++;
}

template <typename T>
inline T *InsertAtIndexPtr(DynamicArray<T> *array, uint32 index) {
    ASSERT(index <= array->count);

    DynamicArrayEnsureCapacity(array, array->count + 1);

    uint32 elementsToMove = array->count - index;
    if (elementsToMove > 0) {
        memmove(array->data + index + 1, array->data + index, sizeof(T) * elementsToMove);
    }

    array->count++;

    T *result = &array->data[index];
    memset(result, 0, sizeof(T));

    return result;
}

template <typename T>
inline void SwapAtIndex(DynamicArray<T> *array, uint32 indexA, uint32 indexB) {
    ASSERT(indexA < array->count);
    ASSERT(indexB < array->count);

    T temp = array->data[indexA];
    array->data[indexA] = array->data[indexB];
    array->data[indexB] = temp;
}

template <typename T>
inline void RemoveAtIndex(DynamicArray<T> *array, uint32 index) {
    ASSERT(index < array->count);

    uint32 elementsToMove = array->count - index - 1;
    if (elementsToMove > 0) {
        memmove(array->data + index, array->data + index + 1, sizeof(T) * elementsToMove);
    }

    array->count--;
}

inline void RemoveAtIndex(uint32 *arrayCapacity, uint32 *arrayCount, void **arrayData, uint32 elemSize, uint32 index) {
    ASSERT(index < *arrayCount);

    uint32 elementsToMove = *arrayCount - index - 1;
    if (elementsToMove > 0) {
        memmove((void *)((u8 *)*arrayData + (index * elemSize)), (void *)((u8 *)*arrayData + ((index + 1) * elemSize)), elemSize * elementsToMove);
    }

    *arrayCount = *arrayCount - 1;
}

template <typename T>
inline void RemoveAtIndexBySwap(DynamicArray<T> *array, uint32 index) {
    ASSERT(index < array->count);

    if (index != array->count - 1) {
        array->data[index] = array->data[array->count - 1];
    }

    array->count--;
}

template <typename T>
// @NOTE: end is exclusive
inline void RemoveRange(DynamicArray<T> *array, uint32 start, uint32 end) {
    ASSERT(end > start);
    uint32 elementsToMove = end - start;
    if (elementsToMove > 0) {
        memmove(array->data + start, array->data + end, sizeof(T) * elementsToMove);        
    }

    array->count -= elementsToMove;
}

template <typename T>
inline bool DynamicArrayEquals(DynamicArray<T> a, DynamicArray<T> b) {
    if (a.count != b.count) {
        return false;
    }

    for (int i = 0; i < a.count; i++) {
        if (!(a[i] == b[i])) {
            return false;
        }
    }

    return true;
}

template <typename T>
inline bool Contains(DynamicArray<T> *array, T element, uint32 *index = NULL) {
    bool result = false;

    for (int i = 0; i < array->count; i++) {
        if ((*array)[i] == element) {
            if (index != NULL) {
                *index = i;
            }
            result = true;
            break;
        }
    }

    return result;
}

template <typename T>
inline bool Contains(DynamicArray<T> *array, T element, bool (*Comparison)(T a, T b) , uint32 *index = NULL) {
    bool result = false;

    for (int i = 0; i < array->count; i++) {
        if (Comparison((*array)[i], element)) {
            if (index != NULL) {
                *index = i;
            }
            result = true;
            break;
        }
    }

    return result;
}

template <typename T>
inline T Last(DynamicArray<T> *array) {
    ASSERT(array->count > 0);

    T result = array->data[array->count - 1];
    return result;
}

template <typename T>
inline T *LastPtr(DynamicArray<T> *array) {
    ASSERT(array->count > 0);

    T *result = &array->data[array->count - 1];
    return result;
}

template <typename T>
inline void RemoveLast(DynamicArray<T> *array) {
    ASSERT(array->count > 0);

    RemoveAtIndex(array, array->count - 1);
}

template <typename T>
void ReverseDynamicArray(DynamicArray<T> *array) {

    u32 last = array->count - 1;
    for (int i = 0; i < array->count / 2; i++) {
        T temp = array->data[i];
        array->data[i] = array->data[last];
        array->data[last] = temp;
        last--;
    }
}

template <typename T>
void QuicksortDynamicArray(DynamicArray<T> *array) {
    Quicksort(array->data, sizeof(T), array->count);
}

template <typename T>
void QuicksortDynamicArray(DynamicArray<T> *array, int32 (*comparator)(const void *a, const void *b)) {
    Quicksort(array->data, sizeof(T), array->count, comparator);
}

template <typename T>
void QuicksortDynamicArray(DynamicArray<T> *array, int32 (*comparator)(const void *a, const void *b, void *data), void *data) {
    Quicksort(array->data, sizeof(T), array->count, comparator, data);
}

template <typename T>
void QuicksortDynamicArray(DynamicArray<T> *array, uint32 count, int32 (*comparator)(const void *a, const void *b, void *data), void *data) {
    Quicksort(array->data, sizeof(T), count, comparator, data);
}

