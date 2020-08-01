
#if _WIN32
#undef FIELD_OFFSET
#endif

#define FIELD_OFFSET(type, field) ((uint64)&(((type *)NULL)->field))

#define POINTER_OFFSET(a, b) ((int32)((uint8 *)(b) - (uint8 *)(a)))
#define BUFFER_OFFSET(buffer, offset) ((uint8 *)(buffer) + (offset))
#define STRIDED_POINTER(type, buffer, stride, index) (type *)((uint8 *)(buffer) + (stride) * (index))
#define STRIDED_READ(type, buffer, stride, index) (*(type *)((uint8 *)(buffer) + (stride) * (index)))

#define ARRAY_LENGTH(type, array) (sizeof(array) / sizeof(type))

struct Str {
    int32 length;
    char *string;
};


uint32 Hash(const char *str) {
    uint32 result = 0;
    uint32 len = strlen(str);
    for (int i = 0; i < len; i++) {
        result = result * 33 + str[i];
    }

    return result;
}
