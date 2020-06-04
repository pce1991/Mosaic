
struct quaternion {
    real32 x, y, z, w;
};

inline quaternion Quaternion(real32 x, real32 y, real32 z, real32 w) {
    quaternion result = {};

    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;

    return result;
}

inline quaternion IdentityQuaternion() {
    return Quaternion(0, 0, 0, 1);
};

// Postive rotation is counter-clockwise when viewing along -axis
// Assumes axis is normalized!
inline quaternion AxisAngle(vec3 axis, real32 rad) {
    axis = Normalize(axis);

    real32 w = cosf(rad * 0.5f);
    real32 a = sinf(rad * 0.5f);
    quaternion result = Quaternion(a * axis.x, a * axis.y, a * axis.z, w);

    return result;
}

inline void ToAxisAngle(quaternion q, vec3 *axis, real32 *rad) {
    *rad = 2 * acosf(q.w);
    axis->x = q.x / sqrtf(1 - Square(q.w));
    axis->y = q.y / sqrtf(1 - Square(q.w));
    axis->z = q.z / sqrtf(1 - Square(q.w));
}

// Euler angles applied in order: heading, attitude, bank.
// [0] http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
// [1] http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
inline quaternion FromEulerAngles(real32 heading, real32 attitude, real32 bank) {
    quaternion result;

    real32 c1 = cosf(heading / 2.0f);
    real32 c2 = cosf(attitude / 2.0f);
    real32 c3 = cosf(bank / 2.0f);

    real32 s1 = sinf(heading / 2.0f);
    real32 s2 = sinf(attitude / 2.0f);
    real32 s3 = sinf(bank / 2.0f);

    result.x = s1 * s2 * c3 + c1 * c2 * s3;
    result.y = s1 * c2 * c3 + c1 * s2 * s3;
    result.z = c1 * s2 * c3 - s1 * c2 * s3;
    result.w = c1 * c2 * c3 - s1 * s2 * s3;
    
    return result;
}

inline quaternion FromEulerAngles(vec3 euler) {
    return FromEulerAngles(euler.y, euler.z, euler.x);
}

inline void ToEulerAngles(quaternion q, real32 *heading, real32 *attitude, real32 *bank) {

    const real32 epsilon = 0.001f;

    real32 sqx = q.x * q.x;
    real32 sqy = q.y * q.y;
    real32 sqz = q.z * q.z;
    real32 sqw = q.w * q.w;

    real32 unit = sqx + sqy + sqz + sqw;
    real32 test = q.x * q.y + q.z * q.w;

    if (test > (0.5f - epsilon) * unit) {
        *heading = 2.0f * atan2f(q.x, q.w);
        *attitude = _PI / 2.0f;
        *bank = 0.0f;
    }
    else if (test < (-0.5f + epsilon) * unit) {
        *heading = -2.0f * atan2f(q.x, q.w);
        *attitude = -_PI / 2.0f;
        *bank = 0.0f;
    }
    else {
        *heading = atan2f(2 * q.y * q.w - 2.0f * q.x * q.z, sqx - sqy - sqz + sqw);
	*attitude = asinf(2.0f * test / unit);
	*bank = atan2f(2.0f * q.x * q.w - 2 * q.y * q.z, -sqx + sqy - sqz + sqw);
    }

    *heading = NormalizeAngleRad(*heading);
    *attitude = NormalizeAngleRad(*attitude);
    *bank = NormalizeAngleRad(*bank);
}

inline vec3 ToEulerAngles(quaternion rotation) {
    vec3 result = {};

    ToEulerAngles(rotation, &result.y, &result.z, &result.x);

    return result;
}

// [0]: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/christian.htm
inline quaternion QuaternionWithXYZ(vec3 X, vec3 Y, vec3 Z) {
    quaternion result;

    real32 trace = X.x + Y.y + Z.z; // I removed + 1.0f; see discussion with Ethan
    if (trace > 0) {
        real32 s = 0.5f / sqrtf(trace + 1.0f);
        result.w = 0.25f / s;
        result.x = (Y.z - Z.y) * s;
        result.y = (Z.x - X.z) * s;
        result.z = (X.y - Y.x) * s;
    }
    else {
        if (X.x > Y.y && X.x > Z.z) {
            float s = 2.0f * sqrtf(1.0f + X.x - Y.y - Z.z);
            result.w = (Y.z - Z.y) / s;
            result.x = 0.25f * s;
            result.y = (Y.x + X.y) / s;
            result.z = (Z.x + X.z) / s;
        }
        else if (Y.y > Z.z) {
            float s = 2.0f * sqrtf(1.0f + Y.y - X.x - Z.z);
            result.w = (Z.x - X.z) / s;
            result.x = (Y.x + X.y) / s;
            result.y = 0.25f * s;
            result.z = (Z.y + Y.z) / s;
        }
        else {
            float s = 2.0f * sqrtf(1.0f + Z.z - X.x - Y.y);
            result.w = (X.y - Y.x) / s;
            result.x = (Z.x + X.z) / s;
            result.y = (Z.y + Y.z) / s;
            result.z = 0.25f * s;
        }
    }

    return result;
}

inline quaternion QuaternionWithXY(vec3 X, vec3 Y) {
    X = Normalize(X);
    vec3 Z = Normalize(Cross(X, Y));
    Y = Normalize(Cross(Z, X));
    return QuaternionWithXYZ(X, Y, Z);
}

inline quaternion QuaternionWithYZ(vec3 Y, vec3 Z) {
    Y = Normalize(Y);
    Z = Normalize(Z);

    vec3 X = Normalize(Cross(Y, Z));
    Y = Cross(Z, X);

    return QuaternionWithXYZ(X, Y, Z);
}

// @TODO: read the Duff paper
inline quaternion QuaternionWithX(vec3 X) {
    quaternion result;

    vec3 Y, Z;
    const real32 epsilon = 1.0e-6;
    if (Abs(Dot(X, V3(0, 1, 0))) < 1.0f - epsilon) {
        Z = Normalize(Cross(X, V3(0, 1, 0)));
        Y = Cross(Z, X);
    }
    else {
        Z = Normalize(Cross(V3(1, 0, 0), X));
        Y = Cross(Z, X);
    }

    result = QuaternionWithXYZ(X, Y, Z);

    return result;
}

bool NearlyEquals(quaternion a, quaternion b, real32 epsilon) {
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon) &&
        NearlyEquals(a.z, b.z, epsilon) &&
        NearlyEquals(a.w, b.w, epsilon);
}

inline real32 LengthSq(quaternion q) {
    real32 result = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    return result;
}

inline real32 Length(quaternion q) {
    real32 result = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return result;
}

inline quaternion Normalize(quaternion q) {
    real32 length = Length(q);
    if (length > 0.0f) {
        return Quaternion(q.x / length, q.y / length, q.z / length, q.w / length);
    }
              
    return Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
}

inline quaternion Inverse(quaternion q) {

    real32 lengthSq = LengthSq(q);
    if (lengthSq < FLT_EPSILON) {
        return {};
    }

    quaternion result = {};
    result.x = -q.x / lengthSq;
    result.y = -q.y / lengthSq;
    result.z = -q.z / lengthSq;
    result.w = q.w / lengthSq;

    return result;
}

inline real32 Dot(quaternion a, quaternion b) {
    real32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

inline real32 Angle(quaternion a, quaternion b) {
    return 2 * acosf(Abs(Dot(a, b)));
}

// a * b applies b before a
inline quaternion operator*(quaternion a, quaternion b) {
    quaternion result = {};

    result.x = (a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y);
    result.y = (a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x);
    result.z = (a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w);
    result.w = (a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);

    return result;
}

inline quaternion operator*(quaternion a, real32 b) {
    quaternion result = {};

    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    result.w = a.w * b;

    return result;
}

inline quaternion operator*(real32 a, quaternion b) {
    quaternion result = {};

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    result.w = a * b.w;

    return result;
}

inline quaternion operator-(quaternion q) {
    quaternion result;

    result.x = -q.x;
    result.y = -q.y;
    result.z = -q.z;
    result.w = -q.w;

    return result;
}

inline quaternion operator+(quaternion a, quaternion b) {
    quaternion result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;

    return result;
}

inline quaternion operator-(quaternion a, quaternion b) {
    quaternion result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;

    return result;
}

// @NOTE: be wary of floating point imprecision here
inline bool operator==(quaternion a, quaternion b) {
    return ((a.x == b.x) && 
            (a.y == b.y) &&
            (a.z == b.z) &&
            (a.w == b.w));
}

inline bool operator!=(quaternion a, quaternion b) {
    return ((a.x != b.x) ||
            (a.y != b.y) ||
            (a.z != b.z) ||
            (a.w != b.w));
}

inline vec3 Rotate(quaternion q, vec3 p) {
    quaternion P = Quaternion(p.x, p.y, p.z, 0.0f);
    quaternion product = q * P * Inverse(q);

    vec3 result = V3(product.x, product.y, product.z);
    return result;
}

// Returns a quaternion q such that a * q = b
inline quaternion RelativeQuaternion(quaternion a, quaternion b) {
    quaternion result = Inverse(b) * a;
    return result;
}

inline quaternion Lerp(quaternion a, quaternion b, real32 t) {
    quaternion result = Quaternion(Lerp(a.x, b.x, t),
                                   Lerp(a.y, b.y, t),
                                   Lerp(a.z, b.z, t),
                                   Lerp(a.w, b.w, t));

    result = Normalize(result);
    return result;
}

// [0]: https://en.wikipedia.org/wiki/Slerp#Quaternion_Slerp
inline quaternion Slerp(quaternion a, quaternion b, real32 t) {
    a = Normalize(a);
    b = Normalize(b);

    real32 dot = Dot(a, b);

    // Do a Lerp() in the case that the input quaternions are close to each other
    if (Abs(dot) > 0.9995) {
        return Lerp(a, b, t);
    }

    // If the dot product is negative the Slerp() won't take the shorter path
    if (dot < 0.0f) {
        b = -b;
        dot = -dot;
    }

    dot = Clamp(dot, -1.0f, 1.0f);
    real32 theta0 = acosf(dot);
    real32 theta = theta0 * t;

    quaternion c = Normalize(b - a * dot);

    quaternion result = a * cosf(theta) + c * sinf(theta);
    return result;
}

bool HasNaN(quaternion q) {
    return isnan(q.x) || isnan(q.y) || isnan(q.z) || isnan(q.w);
}
