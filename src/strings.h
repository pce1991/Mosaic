
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
