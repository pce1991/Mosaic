
struct IntBuffer {
    int32 capacity;
    int32 count;
    int32 *data; 
};

IntBuffer AllocateIntBuffer(int32 capacity) {
    IntBuffer buffer;

    buffer.capacity = capacity;
    buffer.count = 0;

    buffer.data = (int32 *)malloc(capacity);

    return buffer;
}

int32 IntBufferPushBack(IntBuffer *buffer, int32 n) {
    buffer->data[buffer->count++] = n;

    return buffer->count;
}

void IntBufferClear(IntBuffer *buffer) {
    buffer->count = 0;
}
