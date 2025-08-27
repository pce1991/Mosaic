
inline bool IsSpace(uint32 codepoint) {
    return (codepoint == 32);
}

inline bool IsNewline(uint32 codepoint) {
    return (codepoint == 10 || codepoint == 13);
}

inline bool IsTab(uint32 codepoint) {
    return (codepoint == 9);
}

inline bool IsBackspace(uint32 codepoint) {
    return (codepoint == 8);
}

inline bool IsWhitespace(uint32 codepoint) {
    return IsSpace(codepoint) || IsNewline(codepoint);
}

inline bool IsControlCode(uint32 codepoint) {
    return codepoint <= 32 || (codepoint >= 127 && codepoint <= 159);
}

inline bool IsPrintable(uint32 codepoint) {
    return 32 <= codepoint && codepoint <= 126;
}

inline bool IsPunctuation(uint32 c) {
    return (c >= 33 && c <= 46) || (c >= 58 && c <= 64) || (c >= 91 && c <= 96);
}

inline bool IsDigit(uint32 codepoint) {
    return codepoint >= 48 && codepoint <= 57;
}

inline bool IsNumeric(uint32 codepoint) {
    return IsDigit(codepoint) || (codepoint >= 45 && codepoint <= 46);
}

inline bool IsUppercaseASCII(uint32 codepoint) {
    return (codepoint >= 'A') && (codepoint <= 'Z');
}

inline bool IsLowercaseASCII(uint32 codepoint) {
    return (codepoint >= 'a') && (codepoint <= 'z');
}

inline bool IsAlphaASCII(uint32 codepoint) {
    return (((codepoint >= 'a') && (codepoint <= 'z')) ||
            ((codepoint >= 'A') && (codepoint <= 'Z')));
}

inline uint32 UppercaseASCII(uint32 codepoint) {
    if (IsLowercaseASCII(codepoint)) {
        return codepoint - 32;
    }

    return codepoint;
}

inline uint32 LowercaseASCII(uint32 codepoint) {
    if (IsUppercaseASCII(codepoint)) {
        return codepoint + 32;
    }

    return codepoint;
}


inline bool ValidFileSymbol(uint32 codepoint) {
    return IsAlphaASCII(codepoint) || IsDigit(codepoint) || codepoint == '_' || codepoint == '-' || codepoint == '.' || codepoint == '/';
}

inline bool ValidIdentifierSymbol(uint32 codepoint) {
    return IsAlphaASCII(codepoint) || IsDigit(codepoint) || codepoint == '_';
}

uint32 StringToInt32(const char *str) {
    // 10 is the base, not the length of the string
    int64 value = strtoll(str, NULL, 10);
    return (int32)value;
}

uint32 StringToUInt32(const char *str) {
    uint64 value = strtoul(str, NULL, 10);
    return (u32)value;
}

float32 StringToFloat32(const char *str) {
    char *endPtr;
    float32 value = strtof(str, NULL);
    return value;
}


inline bool StringEquals(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return a == b;
    }

    return (strcmp(a, b) == 0);
}

inline bool StringEquals(const char *a, const char *b, uint32 lengthA, uint32 lengthB) {
    if (a == NULL || b == NULL) {
        return a == b;
    }

    if (lengthA != lengthB) {
        return false;
    }

    return memcmp(a, b, lengthA) == 0;
}

inline bool StringEquals(const char *a, const char *b, uint32 length) {
    if (a == NULL || b == NULL) {
        return a == b;
    }

    return memcmp(a, b, length) == 0;
}
