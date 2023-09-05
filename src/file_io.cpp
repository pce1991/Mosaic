
enum FileMode {
               FileMode_Read,
               FileMode_Write,
};

struct FileHandle {
    FILE *file;

    FileMode mode;

    uint64 size;
    uint64 offset;

    u8 *data;
};

bool OpenFileForRead(char *path, FileHandle *file, MAllocator *alloc) {
    file->file = fopen(path, "rb");
    file->mode = FileMode_Read;
    
    if (file->file == NULL) {
        return false;
    }

    fseek(file->file, 0, SEEK_END);

    file->size = ftell(file->file);
    file->offset = 0;

    fseek(file->file, 0, SEEK_SET);

    file->data = (u8 *)alloc->allocate(alloc, file->size);
    fread(file->data, 1, file->size, file->file);
    
    return true;
}

bool OpenFileForRead(char *path, FileHandle *file) {
    file->file = fopen(path, "rb");
    file->mode = FileMode_Read;
    
    if (file->file == NULL) {
        return false;
    }

    fseek(file->file, 0, SEEK_END);

    file->size = ftell(file->file);
    file->offset = 0;

    fseek(file->file, 0, SEEK_SET);

    file->data = (u8 *)malloc(file->size);
    fread(file->data, 1, file->size, file->file);
    
    return true;
}

void CloseFile(FileHandle *file) {
    if (file->mode == FileMode_Write) {
        fwrite(file->data, 1, file->offset, file->file);
    }
    
    fclose(file->file);
}


void FileSeek(FileHandle *file, uint64 offset) {
    file->offset = offset;
}

u8 ReadByte(FileHandle *file) {
    u8 c = file->data[file->offset];

    if (c != 0) {
        file->offset++;
    }

    return c;
}

u8 PeekByte(FileHandle *file) {
    u8 result = file->data[file->offset];

    return result;
}

char ReadChar(FileHandle *file) {
    char c = ReadByte(file);
    return c;
}

char PeekChar(FileHandle *file) {
    return PeekByte(file);
}

uint64 ReadBytes(FileHandle *file, u64 count, void *ptr) {
    u32 read = 0;
    u64 end = file->offset + count;
    end = Min(end, file->size);

    u64 bytesToRead = end - file->offset;

    memcpy(ptr, file->data + file->offset, bytesToRead);
    file->offset += bytesToRead;
    
    return bytesToRead;
}

void ReadInt32(FileHandle *file, int32 *ptr) {
    u32 read = ReadBytes(file, 4, ptr);
}

void ReadReal32(FileHandle *file, real32 *ptr) {
    u32 read = ReadBytes(file, 4, ptr);
}


inline bool ConsumeByteMatching(FileHandle *handle, char c) {
    if (((uint8 *)handle->data)[handle->offset] == c) {
        handle->offset++;
        return true;
    }

    return false;
}


inline bool ConsumeBytesMatching(FileHandle *handle, uint8 *bytes, uint32 len) {
    uint32 prevOffset = handle->offset;
    
    for (int i = 0; i < len; i++) {
        if (!ConsumeByteMatching(handle, bytes[i])) {
            handle->offset = prevOffset;
            return false;
        }
    }

    return true;
}

inline bool ConsumeBytesMatching(FileHandle *handle, const char *str) {
    int32 length = strlen(str);
    return ConsumeBytesMatching(handle, (uint8 *)str, length);
}


// We allocate space to write into, but only write that data into the actual file when we close.
// This minimizes disk access, but it does mean we want to know our maximum size up front.
bool OpenFileForWrite(char *path, FileHandle *file, MAllocator *alloc, u32 size) {
    *file = {};
    file->file = fopen(path, "wb");
    file->mode = FileMode_Write;
    file->offset = 0;
    
    if (file->file == NULL) {
        return false;
    }

    file->size = size;
    file->data = (u8 *)alloc->allocate(alloc, file->size);
    
    return true;
}

// We allocate space to write into, but only write that data into the actual file when we close.
// This minimizes disk access, but it does mean we want to know our maximum size up front.
bool OpenFileForWrite(char *path, FileHandle *file, u32 size) {
    *file = {};
    file->file = fopen(path, "wb");
    file->mode = FileMode_Write;
    file->offset = 0;
    
    if (file->file == NULL) {
        return false;
    }

    file->size = size;
    file->data = (u8 *)malloc(file->size);
    
    return true;
}

void WriteByte(FileHandle *file, u8 byte) {
    file->data[file->offset] = byte;

    if (file->offset < file->size - 1) {
        file->offset++;
    }
}

void WriteChar(FileHandle *file, char c) {
    WriteByte(file, c);
}

uint64 WriteBytes(FileHandle *file, u8 *bytes, u64 count) {
    u32 wrote = 0;
    u64 end = file->offset + count;
    end = Min(end, file->size);

    u64 toWrite = file->size - end;

    memcpy(file->data + file->offset, bytes, toWrite);

    return toWrite;
}


inline void WriteInt32(FileHandle *file, int32 value) {
    ASSERT(file->data != NULL);

    uint64 newOffset = file->offset + 4;
    ASSERT(newOffset < file->size);

    for (int i = 0; i < 4; i++) {
        uint8 byte = (value & (0xFF << (8 * i))) >> (8 * i);
        *((uint8 *)file->data + file->offset++) = byte;
    }
}

inline void WriteReal32(FileHandle *file, real32 value) {
    ASSERT(file->data != NULL);

    uint64 newOffset = file->offset + 4;
    ASSERT(newOffset < file->size);

    uint8 bytes[4];
    memcpy(bytes, &value, sizeof(real32));

    for (int i = 0; i < 4; i++) {
        *((uint8 *)file->data + file->offset++) = bytes[i];
    }
}


