

#define Bytes(n) (n)
#define Kilobytes(n) (1024 * Bytes(n))
#define Megabytes(n) (1024 * Kilobytes(n))
#define Gigabytes(n) (1024 * Megabytes(n))

struct MemoryArena {
    uint32 capacity;
    uint32 size; // in bytes
    void *ptr;
};

void AllocateMemoryArena(MemoryArena *arena, uint32 capacity) {
    arena->capacity = capacity;
    arena->size = 0;
    arena->ptr = malloc(capacity);
    memset(arena->ptr, 0, capacity);
}

void *PushSizeMemoryArena(MemoryArena *arena, uint32 size) {
    void *result = (uint8 *)arena->ptr + arena->size;
    arena->size += size;

    assert(arena->size < arena->capacity);
    
    return result;
}

void ClearMemoryArena(MemoryArena *arena) {
    arena->size = 0;
}

#define PushSize(arena, type, count) (type *)PushSizeMemoryArena(arena, sizeof(type) * count)


struct Str {
    int32 count;
    char *str;
};

