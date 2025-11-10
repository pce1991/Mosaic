
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


struct Token {
  int32 type;

  char *start;
  int32 length;
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

typedef bool CharacterTest(uint32 codepoint);
inline bool ConsumeBytePassing(FileHandle *handle, CharacterTest *Test) {
  if (Test(((uint8 *)handle->data)[handle->offset])) {
    handle->offset++;
    return true;
  }

  return false;
}

inline u32 ConsumeBytesPassing(FileHandle *handle, CharacterTest *Test) {
  u32 len = 0;
  char c = 0;

  while (ConsumeBytePassing(handle, Test)) {
    len++;

    if (handle->offset >= handle->size) {
      break;
    }
  }
  return len;
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

    u64 toWrite = file->offset - end;

    memcpy(file->data + file->offset, bytes, toWrite);
    
    file->offset += toWrite;

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


inline bool ConsumeIntLiteral(FileHandle *file, char **start, int32 *len) {
  char c = ((uint8 *)file->data)[file->offset];

  bool isInt = false;

  if (PeekChar(file) == '-') {
    *start = (char *)(((uint8 *)file->data)) + file->offset;

    if (file->offset + 1 < file->size) {
      char next = *((char *)(((uint8 *)file->data)) + file->offset + 1);

      if (IsDigit(next)) {
        (*len)++;
        ReadChar(file);
        isInt = true;
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }

  while (ConsumeBytePassing(file, &IsDigit)) {
    if (!isInt) {
      *start = (char *)(((uint8 *)file->data)) + file->offset - 1;

      isInt = true;
    }

    (*len)++;
  }

  return isInt;
}


inline bool ConsumeFloatLiteral(FileHandle *file, char **start, int32 *len) {
  char c = ((uint8 *)file->data)[file->offset];

  bool isFloat = false;

  u32 startOffset = file->offset;

  if (PeekChar(file) == '-') {
    *start = (char *)(((uint8 *)file->data)) + file->offset;

    if (file->offset + 1 < file->size) {
      char next = *((char *)(((uint8 *)file->data)) + file->offset + 1);

      if (IsDigit(next)) {
        (*len)++;
        ReadChar(file);
        isFloat = true;
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }

  while (ConsumeBytePassing(file, &IsDigit)) {
    if (!isFloat) {
      *start = (char *)(((uint8 *)file->data)) + file->offset - 1;

      isFloat = true;
    }
    (*len)++;
  }

  if (ConsumeByteMatching(file, '.')) {
    (*len)++;
        
    while (ConsumeBytePassing(file, &IsDigit)) {
      (*len)++;
    }   
  }
  else {
    file->offset = startOffset;
    isFloat = false;
  }

  return isFloat;
}

inline bool ConsumeStringLiteral(FileHandle *file, char **start, uint32 *len) {
  char c = ((uint8 *)file->data)[file->offset];

  bool isString = false;

  *len = 0;

  if (PeekChar(file) == '\"') {
    *start = (char *)(((uint8 *)file->data)) + file->offset;
    ReadChar(file);
        
    char next = *((char *)(((uint8 *)file->data)) + file->offset);

    if (next != '\"') {
      *start = ((char *)(((uint8 *)file->data)) + file->offset);
    }
    else {
      ReadChar(file);
      return true;
    }
        
    isString = true;

    char c = next;
    while (c != '\"') {
      *len = *len + 1;
      c = ReadChar(file);
    }

    *len = *len - 1;

    // To consume the last quote string
    ReadChar(file);
  }
  else {
    isString = false;
  }

  return isString;
}

bool ConsumeIdentifierToken(FileHandle *file, char **tokenStart, int32 *tokenLen) {
  char firstChar = ((uint8 *)file->data)[file->offset];

  bool isIdentifier = false;

  while (IsAlphaASCII(firstChar) && ConsumeBytePassing(file, &ValidIdentifierSymbol)) {
    if (!isIdentifier) {
      *tokenStart = (char *)(((uint8 *)file->data)) + file->offset - 1;

      isIdentifier = true;
    }

    *tokenLen = *tokenLen + 1;
  }

  return isIdentifier;
}

inline u32 ReadBytesUntilWhitespace(FileHandle *handle, char *word, uint32 wordSize = 0) {
  u32 len = 0;
  char c = 0;
  while (!IsWhitespace(c)) {
    uint32 bytesRead = ReadBytes(handle, 1, &c);

    if (bytesRead == 0) { break; }

    if (!IsWhitespace(c)) {
      if (word) {
        ASSERT(wordSize + 1 < len);
        word[len++] = c;
      }
    }
  }

  return len;
}
