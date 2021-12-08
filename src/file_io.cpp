
// memory maps files, should we?

// ConsumeMatching
// ConsumeFloatLiteral
// ConsumeIntLiteral

// TODO: Write

struct FileHandle {
    FILE *file;

    uint64 size;
    uint64 offset;
};

bool OpenFileForRead(char *path, FileHandle *file) {
    file->file = fopen(path, "rb");
    
    if (file->file == NULL) {
        return false;
    }

    fseek(file->file, 0, SEEK_END);

    file->size = ftell(file->file);
    file->offset = 0;

    fseek(file->file, 0, SEEK_SET);
    
    return true;
}

void CloseFile(FileHandle *file) {
    fclose(file->file);
}


void FileSeek(FileHandle *file, uint64 offset) {
    fseek(file->file, offset, SEEK_SET);
    file->offset = offset;
}

u8 ReadByte(FileHandle *file) {
    u8 c = getc(file->file);

    if (c != 0) {
        file->offset++;
    }

    return c;
}

u8 PeekByte(FileHandle *file) {
    u8 result = getc(file->file);
    ungetc(result, file->file);

    return result;
}

char ReadChar(FileHandle *file) {
    char c = getc(file->file);

    if (c != 0) {
        file->offset++;
    }
    
    return c;
}

char PeekChar(FileHandle *file) {
    char result = getc(file->file);
    ungetc(result, file->file);

    return result;
}

u32 ReadBytes(FileHandle *file, u32 count, void *ptr) {
    u32 read = fread(ptr, 1, count, file->file);
    
    file->offset += read;
    
    return read;
}

void ReadInt32(FileHandle *file, int32 *ptr) {
    u32 read = fread(ptr, sizeof(int32), 1, file->file);
    
    file->offset += read;
}

void ReadReal32(FileHandle *file, real32 *ptr) {
    u32 read = fread(ptr, sizeof(real32), 1, file->file);
    
    file->offset += read;
}
