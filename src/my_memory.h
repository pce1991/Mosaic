

#define Bytes(n) (n)
#define Kilobytes(n) (1024 * Bytes(n))
#define Megabytes(n) (1024 * Kilobytes(n))
#define Gigabytes(n) (1024 * Megabytes(n))

// @TODO:
// clearer name for size.
// PushSize and PushArray
// Function to set the cursor/offset/used
// Allocate in 16
// Should it be able to resize? That just complicates things but it's good to have a
//   demonstration
// DynamicArray (or just use MArray for this?)

struct MAllocator;


typedef void *AllocateFunc(MAllocator *allocator, uint64 size);
typedef void DeallocateFunc(MAllocator *allocator, void *data);

struct MAllocator {
    AllocateFunc *allocate;
    DeallocateFunc *deallocate;
};

inline void *AllocateMem(MAllocator *allocator, uint64 size) {
    return allocator->allocate(allocator, size);
}

inline void DeallocateMem(MAllocator *allocator, void *data) {
    if (allocator->deallocate == NULL) { return; }
    allocator->deallocate(allocator, data);
}


struct MemoryArena : MAllocator {
    uint32 capacity;
    uint32 size; // in bytes
    void *ptr;
};


void *PushSizeMemoryArena(MemoryArena *arena, uint64 size);

void AllocateMemoryArena(MemoryArena *arena, uint64 capacity) {
    void *(*allocate)(MemoryArena *, uint64) = &PushSizeMemoryArena;
    arena->allocate = (AllocateFunc *)allocate;
    arena->deallocate = NULL;

    // size_t align = 8;
    // arena->used = (arena->used + (align - 1)) & ~(align - 1);
    
    arena->capacity = capacity;
    arena->size = 0;
    arena->ptr = malloc(capacity);
    memset(arena->ptr, 0, capacity);
}

void *PushSizeMemoryArena(MemoryArena *arena, uint64 size) {
    void *result = (uint8 *)arena->ptr + arena->size;
    arena->size += size;

    assert(arena->size < arena->capacity);
    
    return result;
}

void ClearMemoryArena(MemoryArena *arena) {
    arena->size = 0;
}

// Using a macro here so we can cast and calculate the size.
#define PushSize(arena, count) (type *)PushSizeMemoryArena(arena, count)
#define PushArray(arena, type, count) (type *)PushSizeMemoryArena(arena, sizeof(type) * count)

// @TODO: PushArray is a better name

inline void *AllocCleared(int32 size) {
    void *d = malloc(size);
    memset(d, 0, size);
    return d;
}




template <typename T>
struct MArray {
    uint32 count;
    uint32 capacity;

    T *data;

    // Compiler complains if this isn't a non-static member function!
    inline T& operator[](const int index) const {
        return data[index];
    }
};


// Default MakeArray uses the heap.
template <typename T>
MArray<T> MakeMArray(uint32 capacity) {
    MArray<T> array = {};
    array.count = 0;
    array.capacity = capacity;

    if (capacity > 0) {
        array.data = (T *)malloc(sizeof(T) * capacity);
    }
    
    return array;
}

template <typename T>
MArray<T> MakeMArray(MAllocator *allocator, uint32 capacity) {
    MArray<T> array = {};
    array.count = 0;
    array.capacity = capacity;

    if (capacity > 0) {
        array.data = (T *)allocator->allocate(allocator, sizeof(T) * capacity);
    }
    
    return array;
}

template <typename T>
void PushBack(MArray<T> *array, T element) {
    if (array->count < array->capacity) {
        array->data[array->count] = element;
        array->count++;
    }
}

template <typename T>
inline void RemoveAtIndexBySwap(MArray<T> *array, uint32 index) {
    ASSERT(index < array->count);

    if (index != array->count - 1) {
        array->data[index] = array->data[array->count - 1];
    }

    array->count--;
}

template <typename T>
inline T Last(MArray<T> *array) {
    ASSERT(array->count > 0);

    T result = array->data[array->count - 1];
    return result;
}


// @TODO: Maybe support the rest of the DynamicArray API
// @TODO: chunked array
