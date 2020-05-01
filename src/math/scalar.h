
inline int32 Powi(int32 base, int32 exp) {
    if (exp < 0) {
        return 0;
    }

    int32 result = 1;
    while (exp) {
        if (exp & 1) {
            result *= base;
        }

        exp >>= 1;
        base *= base;
    }

    return result;
}

inline real32 Snap(real32 value, real32 snapInterval, real32 baseValue = 0.0f) {
    real32 result;
    if (snapInterval > 0.0f) {
        result = baseValue + roundf((value - baseValue) / snapInterval) * snapInterval;
    }
    else {
        result = value;
    }

    return result;
}

inline real32 SnapDown(real32 value, real32 snapInterval, real32 baseValue = 0.0f) {
    real32 result;
    if (snapInterval > 0.0f) {
        result = baseValue + floorf((value - baseValue) / snapInterval) * snapInterval;
    }
    else {
        result = value;
    }

    return result;
}

inline real32 SnapUp(real32 value, real32 snapInterval, real32 baseValue = 0.0f) {
    real32 result;
    if (snapInterval > 0.0f) {
        result = baseValue + ceilf((value - baseValue) / snapInterval) * snapInterval;
    }
    else {
        result = value;
    }

    return result;
}


// @NOTE: this only works with positive ints
inline int32 RoundUpToNearestMultiple(int32 x, int32 mult) {
    return ((x + mult - 1) / mult) * mult;
}


// Returns (-1, 0, 1)
inline int32 Signum(real32 x) {
    int32 result = (0 < x) - (x < 0);
    return result;
}

// Returns (-1, 1)
inline int32 Sign(real32 x) {
    int32 result = (0 <= x) - (x < 0);
    return result;
}

inline real32 Lerp(real32 a, real32 b, real32 t) {
    real32 result = (1 - t) * a + t * b;
    return result;
}

inline real32 InverseLerp(real32 a, real32 b, real32 x) {
    real32 result = 0;

    if (a != b) {
        result = (a - x) / (a - b);
    }

    return result;
}

inline real32 LinearRemap(real32 a, real32 b, real32 x, real32 aPrime, real32 bPrime) {
    real32 t = InverseLerp(a, b, x);
    real32 result = Lerp(aPrime, bPrime, t);

    return result;
}

inline real32 Min(real32 a, real32 b) {
    real32 result = a;

    if (a > b) {
        result = b;
    }

    return result;
}

inline real64 Min(real64 a, real64 b) {
    real64 result = a;

    if (a > b) {
        result = b;
    }

    return result;
}


inline int32 Min(int32 a, int32 b) {
    int32 result = a;

    if (a > b) {
        result = b;
    }

    return result;
}

inline uint32 Min(uint32 a, uint32 b) {
    int32 result = a;

    if (a > b) {
        result = b;
    }

    return result;
}

inline uint64 Min(uint64 a, uint64 b) {
    int64 result = a;

    if (a > b) {
        result = b;
    }

    return result;
}

inline real32 Max(real32 a, real32 b) {
    real32 result = a;

    if (a < b) {
        result = b;
    }

    return result;
}

inline real32 Max(int32 a, real32 b) {
    real32 result = a;

    if (a < b) {
        result = b;
    }

    return result;
}

inline real32 Max(real32 a, int32 b) {
    real32 result = a;

    if (a < b) {
        result = b;
    }

    return result;
}


inline int32 Max(int32 a, int32 b) {
    int32 result = a;

    if (a < b) {
        result = b;
    }

    return result;
}

inline uint32 Max(uint32 a, uint32 b) {
    uint32 result = a;

    if (a < b) {
        result = b;
    }

    return result;
}

inline uint64 Max(uint64 a, uint64 b) {
    uint64 result = a;

    if (a < b) {
        result = b;
    }

    return result;
}

inline real32 Clamp(real32 x, real32 a, real32 b) {
    real32 result = x;

    if (x < a) {
        result = a;
    }

    if (x > b) {
        result = b;
    }

    return result;
}

inline int32 Clamp(int32 x, int32 a, int32 b) {
    int32 result = x;

    if (x < a) {
        result = a;
    }

    if (x > b) {
        result = b;
    }

    return result;
}

inline real32 Clamp01(real32 x) {
    return Clamp(x, 0.0f, 1.0f);
}

inline real32 SafeRatio(real32 a, real32 b, real32 n) {
    real32 result = n;

    if (b != 0.0f) {
        result = a / b;
    }

    return result;
}

inline real32 SafeRatio(real32 a, real32 b, real32 n, real32 epsilon) {
    real32 result = n;

    if (b < 0.0f - epsilon && b > 0.0f + epsilon) {
        result = a / b;
    }

    return result;
}


inline real32 SafeInvert(real32 n) {
    return n != 0.0f ? 1.0f / n : 0.0f;
}

inline real32 NormalizeAngleDeg(real32 a) {
    real32 result = a;

    while (result >= 360.0f) {
        result -= 360.0f;
    }

    while (result < 0) {
        result += 360.0f;
    }

    return result;
}

inline real32 NormalizeAngleRad(real32 a) {
    real32 result = a;

    while (result >= _2PI) {
        result -= _2PI;
    }

    while (result < 0) {
        result += _2PI;
    }

    return result;
}

inline real32 DeltaAngleRad(real32 from, real32 to) {

    real32 result = to - from;
    if (result > _PI) {
        result -= _2PI;
    }

    if (result < -_PI) {
        result += _2PI;
    }

    return result;
}

inline real32 Ceilf(real32 x) {
    return ceilf(x);
}

inline real32 Floorf(real32 x) {
    return floorf(x);
}

inline real32 Roundf(real32 x) {
    return roundf(x);
}

inline real32 Fractf(real32 x) {
    return x - Floorf(x);
}

inline real32 Modf(real32 x, real32 m) {
    real32 result = x - m * Floorf(x / m);
    return result;
}

inline real32 Square(real32 x) {
    real32 result = x * x;
    return result;
}

inline real32 Cube(real32 x) {
    real32 result = x * x * x;
    return result;
}

inline real32 Abs(real32 x) {
    return fabsf(x);
}

inline int32 Abs(int32 x) {
    return abs(x);
}

inline int32 Modi(int32 n, int32 m) {
    if (n == -1) { return m - 1; }
    return n % m;
}

inline real32 SmoothStep(real32 t) {
    real32 result = t * t * (3.0f - 2.0f * t);
    return result;
}

inline real32 SmootherStep(real32 t) {
    real32 result = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    return result;
}

inline real32 DegToRad(real32 deg) {
    real32 result = deg * _PI / 180.0f;
    return result;
}

inline real32 RadToDeg(real32 rad) {
    real32 result = rad * 180.0f / _PI;
    return result;
}

inline real32 Distance(real32 x0, real32 y0, real32 x1, real32 y1) {
    real32 result = sqrtf(Square(x1 - x0) + Square(y1 - y0));
    return result;
}

inline real32 Log(real32 base, real32 x) {
    real32 result = log(x) / log(base);
    return result;
}

// [0] http://realtimecollisiondetection.net/blog/?p=89
inline bool NearlyEquals(real32 a, real32 b, real32 absTolerance, real32 relTolerance) {
    real32 diff = Abs(b - a);
    bool result = diff <= Max(absTolerance, relTolerance * Max(Abs(a), Abs(b)));
    return result;
}

inline bool NearlyEquals(real32 a, real32 b, real32 absTolerance = FLT_EPSILON) {
    real32 relTolerance = absTolerance;
    real32 diff = Abs(b - a);
    bool result = diff <= Max(absTolerance, relTolerance * Max(Abs(a), Abs(b)));
    return result;
}

inline real32 PingPong(real32 t, real32 max) {
    real32 L = 2 * max;
    real32 T = Modf(t, L);

    real32 result;

    if (T >= 0 && T <= max) {
        result = T;
    }
    else {
        result = L - T;
    }

    return result;
}

inline bool InRange(real32 n, real32 min, real32 max) {
    return n >= min && n <= max;
}

