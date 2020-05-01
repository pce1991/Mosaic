

#define Bytes(n) (n)
#define Kilobytes(n) (1024 * Bytes(n))
#define Megabytes(n) (1024 * Kilobytes(n))
#define Gigabytes(n) (1024 * Megabytes(n))

typedef struct {
    uint32 capacity;
    uint32 size; // in bytes
    void *ptr;
} FrameMem;

FrameMem frameMem;


void AllocateFrameMem(uint32 capacity) {
    frameMem.capacity = capacity;
    frameMem.size = 0;
    frameMem.ptr = malloc(capacity);
    memset(frameMem.ptr, 0, capacity);
}

void *PushSizeFrameMem(uint32 size) {
    void *result = (uint8 *)frameMem.ptr + frameMem.size;
    frameMem.size += size;

    assert(frameMem.size < frameMem.capacity);
    
    return result;
}

void ClearFrameMem() {
    frameMem.size = 0;
}

#define PushSizeFrameMem(type, count) (type *)PushSizeFrameMem(sizeof(type) * count)
