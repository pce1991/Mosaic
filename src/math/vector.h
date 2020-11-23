
union vec2 {
    real32 data[2];
    struct { real32 x, y; };

    real32& operator [](const int index) {
        return data[index];
    }
};

union vec3 {
    real32 data[3];
    struct { real32 x, y, z; };
    struct { real32 r, g, b; };
    struct { real32 h, s, v; };
    vec2 xy;

    real32& operator [](const int index) {
        return data[index];
    }
};

union vec4 {
    real32 data[4];
    struct { real32 x, y, z, w; };
    struct { real32 r, g, b, a; };
    struct { real32 h, s, v, _a; };
    vec2 xy;
    vec3 xyz;
    vec3 rgb;
    vec3 hsv;

    real32& operator [](const int index) {
        return data[index];
    }
};

union vec2i {
    int32 data[2];
    struct { int32 x, y; };

    int32& operator [](const int index) {
        return data[index];
    }
};

union vec3i {
    int32 data[3];
    struct { int32 x, y, z; };
    struct { int32 r, g, b; };
    vec2i xy;

    int32& operator [](const int index) {
        return data[index];
    }
};

union vec4i {
    int32 data[4];
    struct { int32 x, y, z, w; };
    struct { int32 r, g, b, a; };
    vec2i xy;
    vec3i xyz;
    vec3i rgb;

    int32& operator [](const int index) {
        return data[index];
    }
};

#define UP V3(0, 1, 0)
#define LEFT V3(1, 0, 0)
#define FORWARD V3(0, 0, 1)

#define XZ(a) V2(a.x, a.z)


// Constructors
inline vec2 V2(real32 x, real32 y) {
    vec2 result = {x, y};

    return result;
}

inline vec2 V2(int32 x, int32 y) {
    vec2 result = {(real32)x, (real32)y};

    return result;
}

inline vec2 V2(uint32 x, uint32 y) {
    vec2 result = {(real32)x, (real32)y};

    return result;
}

inline vec2 V2(real32 x, int32 y) {
    vec2 result = {x, (real32)y};

    return result;
}

inline vec2 V2(int32 x, real32 y) {
    vec2 result = {(real32)x, y};

    return result;
}

inline vec2 V2(uint32 x, real32 y) {
    vec2 result = {(real32)x, y};

    return result;
}

inline vec2 V2(real32 x, uint32 y) {
    vec2 result = {x, (real32)y};

    return result;
}


inline vec2 V2(real64 x, real64 y) {
    vec2 result = {(real32)x, (real32)y};

    return result;
}

inline vec2 V2(vec2i a) {
    vec2 result = {(real32)a.x, (real32)a.y};

    return result;
}

inline vec2i V2i(real32 x, real32 y) {
    vec2i result = {(int32)x, (int32)y};

    return result;
}

inline vec2i V2i(int32 x, int32 y) {
    vec2i result = {x, y};

    return result;
}

inline vec2i V2i(uint32 x, uint32 y) {
    vec2i result = {(int32)x, (int32)y};

    return result;
}

inline vec2i V2i(vec2 a) {
    vec2i result = {(int32)a.x, (int32)a.y};

    return result;
}

inline vec2 V2(real32 n) {
    vec2 result = {n, n};
    return result;
}

inline vec3 V3(real32 x, real32 y, real32 z) {
    vec3 result = {x, y, z};

    return result;
}

inline vec3 V3(real32 x, int32 y, int32 z) {
    vec3 result = {x, (real32)y, (real32)z};

    return result;
}

inline vec3 V3(real32 x, real32 y, int32 z) {
    vec3 result = {x, y, (real32)z};

    return result;
}

inline vec3 V3(int32 x, real32 y, real32 z) {
    vec3 result = {(real32)x, y, z};

    return result;
}

inline vec3 V3(int32 x, real32 y, int32 z) {
    vec3 result = {(real32)x, y, (real32)z};

    return result;
}

inline vec3 V3(int32 x, int32 y, real32 z) {
    vec3 result = {(real32)x, (real32)y, z};

    return result;
}

inline vec3 V3(int32 x, int32 y, int32 z) {
    vec3 result = {(real32)x, (real32)y, (real32)z};

    return result;
}

inline vec3 V3(uint32 x, uint32 y, uint32 z) {
    vec3 result = {(real32)x, (real32)y, (real32)z};

    return result;
}

inline vec3 V3(real32 x, int32 y, real32 z) {
    vec3 result = {x, (real32)y, z};

    return result;
}

inline vec3 V3(vec2 xy, real32 z) {
    vec3 result = {(real32)xy.x, (real32)xy.y, (real32)z};

    return result;
}

inline vec3 V3(vec2 xy, int32 z) {
    vec3 result = {(real32)xy.x, (real32)xy.y, (real32)z};

    return result;
}

inline vec3 V3(vec2 xy, uint32 z) {
    vec3 result = {(real32)xy.x, (real32)xy.y, (real32)z};

    return result;
}

inline vec3 V3(real32 n) {
    vec3 result = {n, n, n};
    return result;
}

inline vec3 V3(int32 n) {
    vec3 result = {(real32)n, (real32)n, (real32)n};
    return result;
}

inline vec3 V3(uint32 n) {
    vec3 result = {(real32)n, (real32)n, (real32)n};
    return result;
}

inline vec3 V3(vec3i a) {
    vec3 result = {(real32)a.x, (real32)a.y, (real32)a.z};

    return result;
}

inline vec3i V3i(real32 x, real32 y, real32 z) {
    vec3i result = {(int32)x, (int32)y, (int32)z};

    return result;
}

inline vec3i V3i(int32 x, int32 y, int32 z) {
    vec3i result = {x, y, z};

    return result;
}

inline vec3i V3i(uint32 x, uint32 y, uint32 z) {
    vec3i result = {(int32)x, (int32)y, (int32)z};

    return result;
}

inline vec3i V3i(vec3 a) {
    vec3i result = {(int32)a.x, (int32)a.y, (int32)a.z};

    return result;
}

inline vec4 V4(real32 a) {
    vec4 result = {a, a, a, a};
    return result;
}

inline vec4 V4(real32 x, real32 y, real32 z, real32 w) {
    vec4 result = {x, y, z, w};

    return result;
}

inline vec4 V4(int32 x, int32 y, int32 z, int32 w) {
    vec4 result = {(real32)x, (real32)y, (real32)z, (real32)w};

    return result;
}

inline vec4 V4(uint32 x, uint32 y, uint32 z, uint32 w) {
    vec4 result = {(real32)x, (real32)y, (real32)z, (real32)w};

    return result;
}

inline vec4 V4(int32 x, int32 y, int32 z, real32 w) {
    vec4 result = {(real32)x, (real32)y, (real32)z, (real32)w};

    return result;
}

inline vec4 V4(vec3 xyz, real32 w) {
    vec4 result = {(real32)xyz.x, (real32)xyz.y, (real32)xyz.z, (real32)w};

    return result;
}

inline vec4 V4(vec3 xyz, int32 w) {
    vec4 result = {(real32)xyz.x, (real32)xyz.y, (real32)xyz.z, (real32)w};

    return result;
}

inline vec4 V4(vec3 xyz, uint32 w) {
    vec4 result = {(real32)xyz.x, (real32)xyz.y, (real32)xyz.z, (real32)w};

    return result;
}

inline vec4 V4(vec4i a) {
    vec4 result = {(real32)a.x, (real32)a.y, (real32)a.z, (real32)a.w};

    return result;
}

inline vec4i V4i(real32 x, real32 y, real32 z, real32 w) {
    vec4i result = {(int32)x, (int32)y, (int32)z, (int32)w};

    return result;
}

inline vec4i V4i(int32 x, int32 y, int32 z, int32 w) {
    vec4i result = {x, y, z, w};

    return result;
}

inline vec4i V4i(uint32 x, uint32 y, uint32 z, uint32 w) {
    vec4i result = {(int32)x, (int32)y, (int32)z, (int32)w};

    return result;
}

inline vec4i V4i(vec4 a) {
    vec4i result = {(int32)a.x, (int32)a.y, (int32)a.z, (int32)a.w};

    return result;
}


// vec2 operations
inline vec2 operator+(vec2 a, vec2 b) {
    vec2 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

inline vec3 operator+(vec3 a, vec3 b) {
    vec3 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline vec4 operator+(vec4 a, vec4 b) {
    vec4 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;

    return result;
}

inline vec2i operator+(vec2i a, vec2i b) {
    vec2i result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

inline vec3i operator+(vec3i a, vec3i b) {
    vec3i result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline vec4i operator+(vec4i a, vec4i b) {
    vec4i result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;

    return result;
}

inline vec2 operator-(vec2 a) {
    vec2 result;

    result.x = -a.x;
    result.y = -a.y;

    return result;
}

inline vec3 operator-(vec3 a) {
    vec3 result;

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}

inline vec4 operator-(vec4 a) {
    vec4 result;

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;

    return result;
}

inline vec2i operator-(vec2i a) {
    vec2i result;

    result.x = -a.x;
    result.y = -a.y;

    return result;
}

inline vec3i operator-(vec3i a) {
    vec3i result;

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}

inline vec4i operator-(vec4i a) {
    vec4i result;

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;

    return result;
}

inline vec2 operator-(vec2 a, vec2 b) {
    vec2 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

inline vec3 operator-(vec3 a, vec3 b) {
    vec3 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

inline vec4 operator-(vec4 a, vec4 b) {
    vec4 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}

inline vec2i operator-(vec2i a, vec2i b) {
    vec2i result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

inline vec3i operator-(vec3i a, vec3i b) {
    vec3i result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

inline vec4i operator-(vec4i a, vec4i b) {
    vec4i result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}

inline vec2 operator*(real32 a, vec2 b) {
    vec2 result;

    result.x = a * b.x;
    result.y = a * b.y;

    return result;
}

inline vec2 operator*(vec2 a, real32 b) {
    vec2 result;

    result.x = a.x * b;
    result.y = a.y * b;

    return result;
}

inline vec3 operator*(real32 a, vec3 b) {
    vec3 result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;

    return result;
}

inline vec3 operator*(vec3 a, real32 b) {
    vec3 result;

    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;

    return result;
}

inline vec4 operator*(real32 a, vec4 b) {
    vec4 result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    result.w = a * b.w;

    return result;
}

inline vec4 operator*(vec4 a, real32 b) {
    vec4 result;

    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    result.w = a.w * b;

    return result;
}

inline vec2i operator*(int32 a, vec2i b) {
    vec2i result;

    result.x = a * b.x;
    result.y = a * b.y;

    return result;
}

inline vec2i operator*(vec2i a, int32 b) {
    vec2i result;

    result.x = a.x * b;
    result.y = a.y * b;

    return result;
}

inline vec3i operator*(int32 a, vec3i b) {
    vec3i result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;

    return result;
}

inline vec3i operator*(vec3i a, int32 b) {
    vec3i result;

    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;

    return result;
}

inline vec4i operator*(int32 a, vec4i b) {
    vec4i result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    result.w = a * b.w;

    return result;
}

inline vec4i operator*(vec4i a, int32 b) {
    vec4i result;

    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    result.w = a.w * b;

    return result;
}

inline vec2 operator/(real32 a, vec2 b) {
    vec2 result;

    result.x = a / b.x;
    result.y = a / b.y;

    return result;
}

inline vec2 operator/(vec2 a, real32 b) {
    vec2 result;

    result.x = a.x / b;
    result.y = a.y / b;

    return result;
}

inline vec3 operator/(real32 a, vec3 b) {
    vec3 result;

    result.x = a / b.x;
    result.y = a / b.y;
    result.z = a / b.z;

    return result;
}

inline vec3 operator/(vec3 a, real32 b) {
    vec3 result;

    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;

    return result;
}

inline vec4 operator/(real32 a, vec4 b) {
    vec4 result;

    result.x = a / b.x;
    result.y = a / b.y;
    result.z = a / b.z;
    result.w = a / b.w;

    return result;
}

inline vec4 operator/(vec4 a, real32 b) {
    vec4 result;

    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    result.w = a.w / b;

    return result;
}

inline vec2i operator/(int32 a, vec2i b) {
    vec2i result;

    result.x = a / b.x;
    result.y = a / b.y;

    return result;
}

inline vec2i operator/(vec2i a, int32 b) {
    vec2i result;

    result.x = a.x / b;
    result.y = a.y / b;

    return result;
}

inline vec3i operator/(int32 a, vec3i b) {
    vec3i result;

    result.x = a / b.x;
    result.y = a / b.y;
    result.z = a / b.z;

    return result;
}

inline vec3i operator/(vec3i a, int32 b) {
    vec3i result;

    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;

    return result;
}

inline vec4i operator/(int32 a, vec4i b) {
    vec4i result;

    result.x = a / b.x;
    result.y = a / b.y;
    result.z = a / b.z;
    result.w = a / b.w;

    return result;
}

inline vec4i operator/(vec4i a, int32 b) {
    vec4i result;

    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    result.w = a.w / b;

    return result;
}

inline bool operator==(vec2 a, vec2 b) {
    bool result = (a.x == b.x &&
                  a.y == b.y);

    return result;
}

inline bool operator==(vec3 a, vec3 b) {
    bool result = (a.x == b.x &&
                  a.y == b.y &&
                  a.z == b.z);

    return result;
}

inline bool operator==(vec4 a, vec4 b) {
    bool result = (a.x == b.x &&
                  a.y == b.y &&
                  a.z == b.z &&
                  a.w == b.w);

    return result;
}

inline bool operator==(vec2i a, vec2i b) {
    bool result = (a.x == b.x &&
                  a.y == b.y);

    return result;
}

inline bool operator==(vec3i a, vec3i b) {
    bool result = (a.x == b.x &&
                  a.y == b.y &&
                  a.z == b.z);

    return result;
}

inline bool operator==(vec4i a, vec4i b) {
    bool result = (a.x == b.x &&
                  a.y == b.y &&
                  a.z == b.z &&
                  a.w == b.w);

    return result;
}

inline bool operator!=(vec2 a, vec2 b) {
    bool result = (a.x != b.x ||
                  a.y != b.y);

    return result;
}

inline bool operator!=(vec3 a, vec3 b) {
    bool result = (a.x != b.x ||
                  a.y != b.y ||
                  a.z != b.z);

    return result;
}

inline bool operator!=(vec4 a, vec4 b) {
    bool result = (a.x != b.x ||
                  a.y != b.y ||
                  a.z != b.z ||
                  a.w != b.w);

    return result;
}

inline bool operator!=(vec2i a, vec2i b) {
    bool result = (a.x != b.x ||
                  a.y != b.y);

    return result;
}

inline bool operator!=(vec3i a, vec3i b) {
    bool result = (a.x != b.x ||
                  a.y != b.y ||
                  a.z != b.z);

    return result;
}

inline bool operator!=(vec4i a, vec4i b) {
    bool result = (a.x != b.x ||
                  a.y != b.y ||
                  a.z != b.z ||
                  a.w != b.w);

    return result;
}

inline vec2 Hadamard(vec2 a, vec2 b) {
    vec2 result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;

    return result;
}

inline vec3 Hadamard(vec3 a, vec3 b) {
    vec3 result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;

    return result;
}

inline vec4 Hadamard(vec4 a, vec4 b) {
    vec4 result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;

    return result;
}

inline vec2i Hadamard(vec2i a, vec2i b) {
    vec2i result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;

    return result;
}

inline vec3i Hadamard(vec3i a, vec3i b) {
    vec3i result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;

    return result;
}

inline vec4i Hadamard(vec4i a, vec4i b) {
    vec4i result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;

    return result;
}

inline real32 Dot(vec2 a, vec2 b) {
    real32 result = a.x * b.x + a.y * b.y;

    return result;
}

inline real32 Dot(vec3 a, vec3 b) {
    real32 result = a.x * b.x + a.y * b.y + a.z * b.z;

    return result;
}

inline real32 Dot(vec4 a, vec4 b) {
    real32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

    return result;
}

inline int32 Dot(vec2i a, vec2i b) {
    int32 result = a.x * b.x + a.y * b.y;

    return result;
}

inline int32 Dot(vec3i a, vec3i b) {
    real32 result = a.x * b.x + a.y * b.y + a.z * b.z;

    return result;
}

inline int32 Dot(vec4i a, vec4i b) {
    real32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

    return result;
}

inline real32 Cross(vec2 a, vec2 b) {
    real32 result;

    // This is the magnitude of the 3D cross product if a and b are
    // assumed to be on the plane z = 0
    result = a.y * b.x - a.x * b.y;

    return result;
}

inline vec3 Cross(vec3 a, vec3 b) {
    vec3 result;

    result.x = a.y * b.z - a.z * b.y;
    result.y = -(a.x * b.z - a.z * b.x);
    result.z = a.x * b.y - a.y * b.x;

    return result;
}

inline vec3 TripleVector(vec3 a, vec3 b, vec3 c) {
    return Cross(Cross(a, b), c);
}

inline vec2 Clamp(vec2 a, vec2 min, vec2 max) {
    vec2 result;

    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);

    return result;
}

inline vec3 Clamp(vec3 a, vec3 min, vec3 max) {
    vec3 result;

    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);
    result.z = Clamp(a.z, min.z, max.z);

    return result;
}

inline vec4 Clamp(vec4 a, vec4 min, vec4 max) {
    vec4 result;

    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);
    result.z = Clamp(a.z, min.z, max.z);
    result.w = Clamp(a.w, min.w, max.w);

    return result;
}

inline vec2i Clamp(vec2i a, vec2i min, vec2i max) {
    vec2i result;

    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);

    return result;
}

inline vec3i Clamp(vec3i a, vec3i min, vec3i max) {
    vec3i result;

    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);
    result.z = Clamp(a.z, min.z, max.z);

    return result;
}

inline vec4i Clamp(vec4i a, vec4i min, vec4i max) {
    vec4i result;

    result.x = Clamp(a.x, min.x, max.x);
    result.y = Clamp(a.y, min.y, max.y);
    result.z = Clamp(a.z, min.z, max.z);
    result.w = Clamp(a.w, min.w, max.w);

    return result;
}

inline vec2 Min(vec2 a, vec2 b) {
    vec2 result;

    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);

    return result;
}

inline vec3 Min(vec3 a, vec3 b) {
    vec3 result;

    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    result.z = Min(a.z, b.z);

    return result;
}

inline real32 Min(vec3 a) {
    return Min(Min(a.x, a.y), a.z);
}

inline vec4 Min(vec4 a, vec4 b) {
    vec4 result;

    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    result.z = Min(a.z, b.z);
    result.w = Min(a.w, b.w);

    return result;
}

inline vec2i Min(vec2i a, vec2i b) {
    vec2i result;

    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);

    return result;
}

inline vec3i Min(vec3i a, vec3i b) {
    vec3i result;

    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    result.z = Min(a.z, b.z);

    return result;
}

inline vec4i Min(vec4i a, vec4i b) {
    vec4i result;

    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    result.z = Min(a.z, b.z);
    result.w = Min(a.w, b.w);

    return result;
}

inline vec2 Max(vec2 a, vec2 b) {
    vec2 result;

    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);

    return result;
}

inline vec3 Max(vec3 a, vec3 b) {
    vec3 result;

    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    result.z = Max(a.z, b.z);

    return result;
}


inline real32 Max(vec3 a) {
    return Max(Max(a.x, a.y), a.z);
}

inline vec4 Max(vec4 a, vec4 b) {
    vec4 result;

    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    result.z = Max(a.z, b.z);
    result.w = Max(a.w, b.w);

    return result;
}

inline vec2i Max(vec2i a, vec2i b) {
    vec2i result;

    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);

    return result;
}

inline vec3i Max(vec3i a, vec3i b) {
    vec3i result;

    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    result.z = Max(a.z, b.z);

    return result;
}

inline vec4i Max(vec4i a, vec4i b) {
    vec4i result;

    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    result.z = Max(a.z, b.z);
    result.w = Max(a.w, b.w);

    return result;
}

inline real32 Length(vec2 a) {
    real32 result = sqrtf(a.x * a.x + a.y * a.y);
    return result;
}

inline real32 Length(vec2i a) {
    real32 result = sqrtf(a.x * a.x + a.y * a.y);
    return result;
}

inline real32 Length(vec3 a) {
    real32 result = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
    return result;
}

inline real32 Length(vec4 a) {
    real32 result = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
    return result;
}

inline real32 LengthSq(vec2 a) {
    real32 result = a.x * a.x + a.y * a.y;
    return result;
}

inline real32 LengthSq(vec3 a) {
    real32 result = a.x * a.x + a.y * a.y + a.z * a.z;
    return result;
}

inline real32 LengthSq(vec4 a) {
    real32 result = a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w;
    return result;
}

inline real32 Distance(vec2 a, vec2 b) {
    return Length(b - a);
}

inline real32 Distance(vec2i a, vec2i b) {
    return Length(b - a);
}

inline real32 Distance(vec3 a, vec3 b) {
    return Length(b - a);
}

inline real32 Distance(vec4 a, vec4 b) {
    return Length(b - a);
}

inline real32 DistanceSq(vec2 a, vec2 b) {
    return LengthSq(b - a);
}

inline real32 DistanceSq(vec3 a, vec3 b) {
    return LengthSq(b - a);
}

inline real32 DistanceSq(vec4 a, vec4 b) {
    return LengthSq(b - a);
}

inline real32 Angle(vec2 a, vec2 b) {
    r32 dot = Clamp(Dot(a, b), -1.0f, 1.0f);
    real32 result = acosf(dot);
    return result;
}

// @NOTE: we assume that a and b are normalized.
// This is because dividing Dot(a, b) by Length(a) * Length(b) when they actually are normalized
// can field a result > |1| which results in NaN.
inline real32 Angle(vec3 a, vec3 b) {
    r32 dot = Clamp(Dot(a, b), -1.0f, 1.0f);
    real32 result = acosf(dot);
    return result;
}

inline vec2 Normalize(vec2 a) {
    vec2 result;

    float length = Length(a);
    result.x = SafeRatio(a.x, length, 0);
    result.y = SafeRatio(a.y, length, 0);

    return result;
}

inline vec3 Normalize(vec3 a) {
    vec3 result;

    float length = Length(a);
    result.x = SafeRatio(a.x, length, 0);
    result.y = SafeRatio(a.y, length, 0);
    result.z = SafeRatio(a.z, length, 0);

    return result;
}

inline vec4 Normalize(vec4 a) {
    vec4 result;

    float length = Length(a);
    result.x = SafeRatio(a.x, length, 0);
    result.y = SafeRatio(a.y, length, 0);
    result.z = SafeRatio(a.z, length, 0);
    result.w = SafeRatio(a.w, length, 0);

    return result;
}

inline vec2 Lerp(vec2 a, vec2 b, real32 t) {
    vec2 result;

    result.x = Lerp(a.x, b.x, t);
    result.y = Lerp(a.y, b.y, t);

    return result;
}

inline vec3 Lerp(vec3 a, vec3 b, real32 t) {
    vec3 result;

    result.x = Lerp(a.x, b.x, t);
    result.y = Lerp(a.y, b.y, t);
    result.z = Lerp(a.z, b.z, t);

    return result;
}

inline vec4 Lerp(vec4 a, vec4 b, real32 t) {
    vec4 result;

    result.x = Lerp(a.x, b.x, t);
    result.y = Lerp(a.y, b.y, t);
    result.z = Lerp(a.z, b.z, t);
    result.w = Lerp(a.w, b.w, t);

    return result;
}

inline vec2 NLerp(vec2 a, vec2 b, real32 t) {
    vec2 result = Normalize(Lerp(a, b, t));

    return result;
}

inline vec3 NLerp(vec3 a, vec3 b, real32 t) {
    vec3 result = Normalize(Lerp(a, b, t));

    return result;
}

inline vec4 NLerp(vec4 a, vec4 b, real32 t) {
    vec4 result = Normalize(Lerp(a, b, t));

    return result;
}

inline vec2 LeftHandPerp(vec2 a) {
    vec2 result = V2(-a.y, a.x);

    return result;
}

inline vec2 RightHandPerp(vec2 a) {
    vec2 result = V2(a.y, -a.x);

    return result;
}

inline vec3 Abs(vec3 v) {
    vec3 result;
    result.x = Abs(v.x);
    result.y = Abs(v.y);
    result.z = Abs(v.z);
    return result;
}

inline int32 LargestComponent(vec3 v, uint32 *min0, uint32 *min1) {
    v.x = Abs(v.x);
    v.y = Abs(v.y);
    v.z = Abs(v.z);

    if (v.x > v.y && v.x > v.z) {
        *min0 = 1;
        *min1 = 2;
        return 0;
    }
    if (v.y > v.x && v.y > v.z) {
        *min0 = 0;
        *min1 = 2;
        return 1;
    }
    if (v.z > v.x && v.z > v.y) {
        *min0 = 0;
        *min1 = 1;
        return 2;
    }

    return -1;
}

inline int32 LargestComponent(vec2 v, int32 *min0) {
    v.x = Abs(v.x);
    v.y = Abs(v.y);

    if (v.x > v.y) {
        *min0 = 1;
        return 0;
    }
    if (v.y > v.x) {
        *min0 = 0;
        return 1;
    }

    return -1;
}

vec2 Cardinal4Clamp(vec2 dir) {
    vec2 result = {};
    
    int32 smallestComp;
    int32 largestComponent = LargestComponent(dir, &smallestComp);

    if (largestComponent == -1) { return result; }

    result.data[largestComponent] = dir.data[largestComponent];
    if (result.data[largestComponent] < 0) {
        result.data[largestComponent] = -1;
    }
    else if (result.data[largestComponent] > 0) {
        result.data[largestComponent] = 1;
    }

    return result; 
}

bool HasNaN(vec3 v) {
    return isnan(v.x) || isnan(v.y) || isnan(v.z);
}


bool NearlyEquals(vec2 a, vec2 b, real32 epsilon) {
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon);
}

bool NearlyEquals(vec3 a, vec3 b, real32 epsilon) {
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon) &&
        NearlyEquals(a.z, b.z, epsilon);
}

bool NearlyEquals(vec4 a, vec4 b, real32 epsilon) {
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon) &&
        NearlyEquals(a.z, b.z, epsilon) &&
        NearlyEquals(a.w, b.w, epsilon);
}


inline vec3 Project(vec3 a, vec3 p) {
    r32 d = Dot(a, p);
    r32 lenSq = LengthSq(a);
    return (d / lenSq) * a;
}

inline vec3 ProjectPointOntoLine(vec3 pt, vec3 a, vec3 b) {
    vec3 dir = Normalize(b - a);
    return a + (dir * Dot(dir, pt - a));
}

vec3 Round(vec3 a) {
    vec3 result = a;
    result.x = Roundf(a.x);
    result.y = Roundf(a.y);
    result.z = Roundf(a.z);
    return result;
}

vec3 Ceilv(vec3 a) {
    vec3 result = a;
    result.x = Ceilf(a.x);
    result.y = Ceilf(a.y);
    result.z = Ceilf(a.z);
    return result;
}

