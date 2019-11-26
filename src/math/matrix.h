
#ifndef _ETC_MATH_MATRIX_H
#define _ETC_MATH_MATRIX_H

union mat3 {
    real32 data[9];
    vec3 columns[3];
    struct {
        real32 m00;
        real32 m10;
        real32 m20;
        real32 m01;
        real32 m11;
        real32 m21;
        real32 m02;
        real32 m12;
        real32 m22;
    };
};

union mat4 {
    real32 data[16];
    vec4 columns[4];
    struct {
        real32 m00;
        real32 m10;
        real32 m20;
        real32 m30;
        real32 m01;
        real32 m11;
        real32 m21;
        real32 m31;
        real32 m02;
        real32 m12;
        real32 m22;
        real32 m32;
        real32 m03;
        real32 m13;
        real32 m23;
        real32 m33;
    };
};

inline mat3 Zero3() {
    mat3 result = {};
    return result;
};

inline mat4 Zero4() {
    mat4 result = {};
    return result;
}

inline mat3 Identity3() {
    mat3 result = {1.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 1.0f};

    return result;
}

inline mat4 Identity4() {
    mat4 result = {1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

inline mat4 Transpose(mat4 m) {
    mat4 m_;
    m_.columns[0] = {m.columns[0][0], m.columns[1][0], m.columns[2][0], m.columns[3][0]};
    m_.columns[1] = {m.columns[0][1], m.columns[1][1], m.columns[2][1], m.columns[3][1]};
    m_.columns[2] = {m.columns[0][2], m.columns[1][2], m.columns[2][2], m.columns[3][2]};
    m_.columns[3] = {m.columns[0][3], m.columns[1][3], m.columns[2][3], m.columns[3][3]};

    return m_;
}

inline vec3 GetX(mat4 a) {
    vec3 result = a.columns[0].xyz;
    return result;
}

inline vec3 GetY(mat4 a) {
    vec3 result = a.columns[1].xyz;
    return result;
}

inline vec3 GetZ(mat4 a) {
    vec3 result = a.columns[2].xyz;
    return result;
}

inline vec3 GetTranslation(mat4 a) {
    vec3 result = a.columns[3].xyz;
    return result;
}

inline vec3 GetScale(mat4 m) {
    vec3 result;
    result.x = Length(m.columns[0].xyz);
    result.y = Length(m.columns[1].xyz);
    result.z = Length(m.columns[2].xyz);

    return result;
}

inline mat3 GetRotation(mat4 m) {
    vec3 scale = GetScale(m);
    
    mat3 r;
#if 0
    r.m00 = m.m00 / scale.x;
    r.m10 = m.m10 / scale.x;
    r.m20 = m.m20 / scale.x;

    r.m01 = m.m01 / scale.y;
    r.m11 = m.m11 / scale.y;
    r.m21 = m.m21 / scale.y;

    r.m02 = m.m02 / scale.z;
    r.m12 = m.m12 / scale.z;
    r.m22 = m.m22 / scale.z;
#else
    r.columns[0][0] = m.columns[0][0] / scale.x;
    r.columns[0][1] = m.columns[0][1] / scale.x;
    r.columns[0][2] = m.columns[0][2] / scale.x;

    r.columns[1][0] = m.columns[1][0] / scale.x;
    r.columns[1][1] = m.columns[1][1] / scale.x;
    r.columns[1][2] = m.columns[1][2] / scale.x;

    r.columns[2][0] = m.columns[2][0] / scale.z;
    r.columns[2][1] = m.columns[2][1] / scale.z;
    r.columns[2][2] = m.columns[2][2] / scale.z;
#endif    
    return r;
}

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
inline quaternion ToQuaternion(mat3 a) {
    quaternion result = {};

    result.w = sqrtf(Max(0.0f, 1.0f + a.columns[0][0] + a.columns[1][1] + a.columns[2][2])) / 2.0f;

    result.x = sqrtf(Max(0.0f, 1.0f + a.columns[0][0] - a.columns[1][1] - a.columns[2][2])) / 2.0f;
    result.y = sqrtf(Max(0.0f, 1.0f - a.columns[0][0] + a.columns[1][1] - a.columns[2][2])) / 2.0f;
    result.z = sqrtf(Max(0.0f, 1.0f - a.columns[0][0] - a.columns[1][1] + a.columns[2][2])) / 2.0f;

    // @NOTE: _copysign(x, y) returns a value with the magnitude of x and the sign of y
    result.x = _copysign(result.x, a.columns[2][1] - a.columns[1][2]);
    result.y = _copysign(result.y, a.columns[0][2] - a.columns[2][0]);
    result.z = _copysign(result.z, a.columns[1][0] - a.columns[0][1]);

    return result;
}


inline vec3 GetX(mat3 a) {
    vec3 result = a.columns[0];
    return result;
}

inline vec3 GetY(mat3 a) {
    vec3 result = a.columns[1];
    return result;
}

inline vec3 GetZ(mat3 a) {
    vec3 result = a.columns[2];
    return result;
}


inline vec3 Row(mat3 *a, int i) {
    vec3 result = V3(a->data[0 + i], a->data[3 + i], a->data[6 + i]);

    return result;
}

inline vec4 Row(mat4 *a, int i) {
    vec4 result = V4(a->data[0 + i], a->data[4 + i], a->data[8 + i], a->data[12 + i]);

    return result;
}

inline void SwapRows(mat3 *a, int i, int j) {
    vec3 oldRowI = Row(a, i);

    a->data[0 + i] = a->data[0 + j];
    a->data[3 + i] = a->data[3 + j];
    a->data[6 + i] = a->data[6 + j];

    a->data[0 + j] = oldRowI.data[0];
    a->data[3 + j] = oldRowI.data[1];
    a->data[6 + j] = oldRowI.data[2];
}

inline void SwapRows(mat4 *a, int i, int j) {
    vec4 oldRowI = Row(a, i);

    a->data[0 + i] = a->data[0 + j];
    a->data[4 + i] = a->data[4 + j];
    a->data[8 + i] = a->data[8 + j];
    a->data[12 + i] = a->data[12 + j];

    a->data[0 + j] = oldRowI.data[0];
    a->data[4 + j] = oldRowI.data[1];
    a->data[8 + j] = oldRowI.data[2];
    a->data[12 + j] = oldRowI.data[3];
}

// rowI -> rowI + scale * rowJ
inline void AddRow(mat3 *a, int i, int j, real32 scale) {
    a->data[0 + i] += scale * a->data[0 + j];
    a->data[3 + i] += scale * a->data[3 + j];
    a->data[6 + i] += scale * a->data[6 + j];
}

// rowI -> rowI + scale * rowJ
inline void AddRow(mat4 *a, int i, int j, real32 scale) {
    a->data[0 + i] += scale * a->data[0 + j];
    a->data[4 + i] += scale * a->data[4 + j];
    a->data[8 + i] += scale * a->data[8 + j];
    a->data[12 + i] += scale * a->data[12 + j];
}

inline void MultiplyRow(mat3 *a, int i, real32 scale) {
    a->data[0 + i] *= scale;
    a->data[3 + i] *= scale;
    a->data[6 + i] *= scale;
}

inline void MultiplyRow(mat4 *a, int i, real32 scale) {
    a->data[0 + i] *= scale;
    a->data[4 + i] *= scale;
    a->data[8 + i] *= scale;
    a->data[12 + i] *= scale;
}

inline bool Inverse(mat3 a, mat3 *inverted) {
    mat3 tmp = a;
    mat3 result = Identity3();

    for (int col = 0; col < 3; col++) {
        int maxRow = col;
        real32 maxRowValue = fabsf(tmp.data[3 * col + col]);
        
        for (int row = col + 1; row < 3; row++) {
            real32 rowValue = fabsf(tmp.data[3 * col + row]);

            if (rowValue > maxRowValue) {
                maxRow = row;
                maxRowValue = rowValue;
            }
        }

        SwapRows(&tmp, maxRow, col);
        SwapRows(&result, maxRow, col);

        float mJJ = tmp.data[3 * col + col];
        if (mJJ == 0) {
            return false;
        }

        MultiplyRow(&tmp, col, 1.0f / mJJ);
        MultiplyRow(&result, col, 1.0f / mJJ);

        for (int row = 0; row < 3; row++) {
            if (row == col) {
                continue;
            }

            float scale = -tmp.data[3 * col + row];
            AddRow(&tmp, row, col, scale);
            AddRow(&result, row, col, scale);
        }
    }

    if (inverted != NULL) {
        *inverted = result;
    }

    return true;
}

inline bool Inverse(mat4 a, mat4 *inverted) {
    mat4 tmp = a;
    mat4 result = Identity4();

    for (int col = 0; col < 4; col++) {
        int maxRow = col;
        real32 maxRowValue = fabsf(tmp.data[4 * col + col]);
        
        for (int row = col + 1; row < 4; row++) {
            real32 rowValue = fabsf(tmp.data[4 * col + row]);

            if (rowValue > maxRowValue) {
                maxRow = row;
                maxRowValue = rowValue;
            }
        }

        SwapRows(&tmp, maxRow, col);
        SwapRows(&result, maxRow, col);

        float mJJ = tmp.data[4 * col + col];
        if (mJJ == 0) {
            return false;
        }

        MultiplyRow(&tmp, col, 1.0f / mJJ);
        MultiplyRow(&result, col, 1.0f / mJJ);

        for (int row = 0; row < 4; row++) {
            if (row == col) {
                continue;
            }

            float scale = -tmp.data[4 * col + row];
            AddRow(&tmp, row, col, scale);
            AddRow(&result, row, col, scale);
        }
    }

    if (inverted != NULL) {
        *inverted = result;
    }

    return true;
}

inline mat4 OrthogonalInverse(mat4 a) {

    vec3 X = a.columns[0].xyz;
    vec3 Y = a.columns[1].xyz;
    vec3 Z = a.columns[2].xyz;
    
    real32 invDet = 1.0f / Dot(Cross(X, Y), Z);

    vec3 row0 = Cross(Y, Z) * invDet;
    vec3 row1 = Cross(Z, X) * invDet;
    vec3 row2 = Cross(X, Y) * invDet;

    
    mat4 result;
    result.data[0] = row0[0];
    result.data[1] = row1[0];
    result.data[2] = row2[0];
    result.data[3] = 0.0f;

    result.data[4] = row0[1];
    result.data[5] = row1[1];
    result.data[6] = row2[1];
    result.data[7] = 0.0f;

    result.data[8] = row0[2];
    result.data[9] = row1[2];
    result.data[10] = row2[2];
    result.data[11] = 0.0f;

    result.data[12] = -(result.data[0] * a.data[12] + result.data[4] * a.data[13] + result.data[8] * a.data[14]);
    result.data[13] = -(result.data[1] * a.data[12] + result.data[5] * a.data[13] + result.data[9] * a.data[14]);
    result.data[14] = -(result.data[2] * a.data[12] + result.data[6] * a.data[13] + result.data[10] * a.data[14]);
    result.data[15] = 1.0f;

    return result;
}

inline mat3 operator*(mat3 a, mat3 b)  {
    mat3 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[3] * b.data[1] + a.data[6] + b.data[2];
    result.data[1] = a.data[1] * b.data[0] + a.data[4] * b.data[1] + a.data[7] + b.data[2];
    result.data[2] = a.data[2] * b.data[0] + a.data[5] * b.data[1] + a.data[8] + b.data[2];

    result.data[3] = a.data[0] * b.data[3] + a.data[3] * b.data[4] + a.data[6] + b.data[5];
    result.data[4] = a.data[1] * b.data[3] + a.data[4] * b.data[4] + a.data[7] + b.data[5];
    result.data[5] = a.data[2] * b.data[3] + a.data[5] * b.data[4] + a.data[8] + b.data[5];

    result.data[6] = a.data[0] * b.data[6] + a.data[3] * b.data[7] + a.data[6] + b.data[8];
    result.data[7] = a.data[1] * b.data[6] + a.data[4] * b.data[7] + a.data[7] + b.data[8];
    result.data[8] = a.data[2] * b.data[6] + a.data[5] * b.data[7] + a.data[8] + b.data[8];
    
    return result;
}

inline mat4 operator*(mat4 a, mat4 b) {
    mat4 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[4] * b.data[1] + a.data[8] * b.data[2] + a.data[12] * b.data[3];
    result.data[1] = a.data[1] * b.data[0] + a.data[5] * b.data[1] + a.data[9] * b.data[2] + a.data[13] * b.data[3];
    result.data[2] = a.data[2] * b.data[0] + a.data[6] * b.data[1] + a.data[10] * b.data[2] + a.data[14] * b.data[3];
    result.data[3] = a.data[3] * b.data[0] + a.data[7] * b.data[1] + a.data[11] * b.data[2] + a.data[15] * b.data[3];

    result.data[4] = a.data[0] * b.data[4] + a.data[4] * b.data[5] + a.data[8] * b.data[6] + a.data[12] * b.data[7];
    result.data[5] = a.data[1] * b.data[4] + a.data[5] * b.data[5] + a.data[9] * b.data[6] + a.data[13] * b.data[7];
    result.data[6] = a.data[2] * b.data[4] + a.data[6] * b.data[5] + a.data[10] * b.data[6] + a.data[14] * b.data[7];
    result.data[7] = a.data[3] * b.data[4] + a.data[7] * b.data[5] + a.data[11] * b.data[6] + a.data[15] * b.data[7];

    result.data[8] = a.data[0] * b.data[8] + a.data[4] * b.data[9] + a.data[8] * b.data[10] + a.data[12] * b.data[11];
    result.data[9] = a.data[1] * b.data[8] + a.data[5] * b.data[9] + a.data[9] * b.data[10] + a.data[13] * b.data[11];
    result.data[10] = a.data[2] * b.data[8] + a.data[6] * b.data[9] + a.data[10] * b.data[10] + a.data[14] * b.data[11];
    result.data[11] = a.data[3] * b.data[8] + a.data[7] * b.data[9] + a.data[11] * b.data[10] + a.data[15] * b.data[11];

    result.data[12] = a.data[0] * b.data[12] + a.data[4] * b.data[13] + a.data[8] * b.data[14] + a.data[12] * b.data[15];
    result.data[13] = a.data[1] * b.data[12] + a.data[5] * b.data[13] + a.data[9] * b.data[14] + a.data[13] * b.data[15];
    result.data[14] = a.data[2] * b.data[12] + a.data[6] * b.data[13] + a.data[10] * b.data[14] + a.data[14] * b.data[15];
    result.data[15] = a.data[3] * b.data[12] + a.data[7] * b.data[13] + a.data[11] * b.data[14] + a.data[15] * b.data[15];

    return result;
}

inline vec3 operator*(mat3 a, vec3 b) {
    vec3 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[3] * b.data[1] + a.data[6] * b.data[2];
    result.data[1] = a.data[1] * b.data[0] + a.data[4] * b.data[1] + a.data[7] * b.data[2];
    result.data[2] = a.data[2] * b.data[0] + a.data[5] * b.data[1] + a.data[8] * b.data[2];

    return result;
}

inline vec4 operator*(mat4 a, vec4 b) {
    vec4 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[4] * b.data[1] + a.data[8] * b.data[2] + a.data[12] * b.data[3];
    result.data[1] = a.data[1] * b.data[0] + a.data[5] * b.data[1] + a.data[9] * b.data[2] + a.data[13] * b.data[3];
    result.data[2] = a.data[2] * b.data[0] + a.data[6] * b.data[1] + a.data[10] * b.data[2] + a.data[14] * b.data[3];
    result.data[3] = a.data[3] * b.data[0] + a.data[7] * b.data[1] + a.data[11] * b.data[2] + a.data[15] * b.data[3];

    return result;
}

inline vec3 MultiplyDirection(mat4 a, vec3 b) {
    vec3 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[4] * b.data[1] + a.data[8] * b.data[2];
    result.data[1] = a.data[1] * b.data[0] + a.data[5] * b.data[1] + a.data[9] * b.data[2];
    result.data[2] = a.data[2] * b.data[0] + a.data[6] * b.data[1] + a.data[10] * b.data[2];

    result = Normalize(result);

    return result;
}

inline vec3 MultiplyPoint(mat4 a, vec3 b) {
    vec3 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[4] * b.data[1] + a.data[8] * b.data[2] + a.data[12];
    result.data[1] = a.data[1] * b.data[0] + a.data[5] * b.data[1] + a.data[9] * b.data[2] + a.data[13];
    result.data[2] = a.data[2] * b.data[0] + a.data[6] * b.data[1] + a.data[10] * b.data[2] + a.data[14];

    return result;
}

// Left multiplies by a row vector, resulting in a row vector
inline vec4 LeftMultiply(vec4 a, mat4 b) {
    vec4 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[1] * b.data[1] + a.data[2] * b.data[2] + a.data[3] * b.data[3];
    result.data[1] = a.data[0] * b.data[4] + a.data[1] * b.data[5] + a.data[2] * b.data[6] + a.data[3] * b.data[7];
    result.data[2] = a.data[0] * b.data[8] + a.data[1] * b.data[9] + a.data[2] * b.data[10] + a.data[3] * b.data[11];
    result.data[3] = a.data[0] * b.data[12] + a.data[1] * b.data[13] + a.data[2] * b.data[14] + a.data[3] * b.data[15];

    return result;
}

inline vec3 LeftMultiply(vec3 a, mat3 b) {
    vec3 result;

    result.data[0] = a.data[0] * b.data[0] + a.data[1] * b.data[1] + a.data[2] * b.data[2];
    result.data[1] = a.data[0] * b.data[3] + a.data[1] * b.data[4] + a.data[2] * b.data[5];
    result.data[2] = a.data[0] * b.data[6] + a.data[1] * b.data[7] + a.data[2] * b.data[8];

    return result;
}

inline vec3 ProjectPoint(mat4 a, vec3 b) {
    vec4 homogeneous = a * V4(b, 1.0f);
    vec3 result = homogeneous.xyz / homogeneous.w;
    return result;
}


// Constructors
inline mat4 Translation4(vec3 translation) {
    mat4 result = {1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f,
                   translation.x, translation.y, translation.z, 1.0f};

    return result;
}

inline mat4 Rotation4(quaternion q) {
    mat4 result;

    result.columns[0] = V4(1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z,
                           2.0f * (q.x * q.y + q.w * q.z),
                           2.0f * (q.x * q.z - q.w * q.y),
                           0.0f);
    result.columns[1] = V4(2.0f * (q.x * q.y - q.w * q.z),
                           1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z,
                           2.0f * (q.y * q.z + q.w * q.x),
                           0.0f);
    result.columns[2] = V4(2.0f * (q.x * q.z + q.w * q.y),
                           2.0f * (q.y * q.z - q.w * q.x),
                           1.0f - 2.0f * (q.x * q.x + q.y * q.y),
                           0.0f);
    result.columns[3] = V4(0.0f, 0.0f, 0.0f, 1.0f);

    return result;
}

inline mat4 Scale4(real32 scale) {
    mat4 result = {scale, 0.0f, 0.0f, 0.0f,
                   0.0f, scale, 0.0f, 0.0f,
                   0.0f, 0.0f, scale, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

inline mat4 Scale4(vec3 scale) {
    mat4 result = {scale.x, 0.0f, 0.0f, 0.0f,
                   0.0f, scale.y, 0.0f, 0.0f,
                   0.0f, 0.0f, scale.z, 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

inline mat3 Translation3(vec3 translation) {
    mat3 result = {1.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f,
                   translation.x, translation.y, 1.0f};

    return result;
}

inline mat3 Rotation3(quaternion q) {
    mat3 result;

    result.columns[0] = V3(1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z,
                           2.0f * (q.x * q.y + q.w * q.z),
                           2.0f * (q.x * q.z - q.w * q.y));
    result.columns[1] = V3(2.0f * (q.x * q.y - q.w * q.z),
                           1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z,
                           2.0f * (q.y * q.z + q.w * q.x));
    result.columns[2] = V3(2.0f * (q.x * q.z + q.w * q.y),
                           2.0f * (q.y * q.z - q.w * q.x),
                           1.0f - 2.0f * (q.x * q.x + q.y * q.y));

    return result;
}

inline mat3 Scale3(real32 scale) {
    mat3 result = {scale, 0.0f, 0.0f,
                   0.0f, scale, 0.0f,
                   0.0f, 0.0f, scale};

    return result;
}

inline mat3 Scale3(vec3 scale) {
    mat3 result = {scale.x, 0.0f, 0.0f,
                   0.0f, scale.y, 0.0f,
                   0.0f, 0.0f, scale.z};

    return result;
}

inline mat4 TRS(vec3 translation, mat4 rotation, real32 scale) {
    mat4 result = rotation * Scale4(scale);

    result.columns[3].x = translation.x;
    result.columns[3].y = translation.y;
    result.columns[3].z = translation.z;

    return result;
}

inline mat4 TRS(vec3 translation, quaternion rotation, real32 scale) {
    mat4 result = Rotation4(rotation) * Scale4(scale);

    result.columns[3].x = translation.x;
    result.columns[3].y = translation.y;
    result.columns[3].z = translation.z;

    return result;
}

inline mat4 TRS(vec3 translation, quaternion rotation, vec3 scale) {
    mat4 result = Rotation4(rotation) * Scale4(scale);

    result.columns[3].x = translation.x;
    result.columns[3].y = translation.y;
    result.columns[3].z = translation.z;

    return result;
}

inline mat4 TS(vec3 translation, real32 scale) {
    mat4 result = Scale4(scale);

    result.columns[3].x = translation.x;
    result.columns[3].y = translation.y;
    result.columns[3].z = translation.z;

    return result;
}

inline mat4 TS(vec3 translation, vec3 scale) {
    mat4 result = Scale4(scale);

    result.columns[3].x = translation.x;
    result.columns[3].y = translation.y;
    result.columns[3].z = translation.z;

    return result;
}

inline mat4 TS(vec2 translation, vec2 scale) {
    mat4 result = Scale4(V3(scale, 1));

    result.columns[3].x = translation.x;
    result.columns[3].y = translation.y;

    return result;
}

inline mat4 RS(quaternion rotation, vec3 scale) {
    return Rotation4(rotation) * Scale4(scale);
}

inline mat4 Orthographic(real32 left, real32 right, real32 bottom, real32 top, real32 nearPlane, real32 farPlane) {
    mat4 result;

    result.columns[0] = V4(2.0f / (right - left), 0.0f, 0.0f, 0.0f);
    result.columns[1] = V4(0.0f, 2.0f / (top - bottom), 0.0f, 0.0f);
    result.columns[2] = V4(0.0f, 0.0f, -2.0f / (farPlane - nearPlane), 0.0f);
    result.columns[3] = V4(-(right + left) / (right - left),
                           -(top + bottom) / (top - bottom),
                           -(farPlane + nearPlane) / (farPlane - nearPlane),
                           1.0f);

    return result;
}


// Points down +Z, with right-handed coordinate system
inline mat4 Perspective(real32 vFOV, real32 aspect, real32 nearPlane, real32 farPlane) {

    real32 top = nearPlane * tanf(vFOV * 0.5f);
    real32 bottom = -top;

    real32 left = -aspect * top;
    real32 right = -left;

    mat4 result;

    result.columns[0] = V4(2 * nearPlane / (left - right), 0.0f, 0.0f, 0.0f);
    result.columns[1] = V4(0.0f, 2 * nearPlane / (top - bottom), 0.0f, 0.0f);
    result.columns[2] = -V4((left + right) / (left - right), (top + bottom) / (top - bottom), -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0f);
    result.columns[3] = V4(0.0f, 0.0f, -(2 * nearPlane * farPlane) / (farPlane - nearPlane), 0.0f);

    return result;
}

inline mat4 PerspectiveInfiniteFarPlane(real32 vFOV, real32 aspect, real32 nearPlane) {
    real32 top = nearPlane * tanf(vFOV * 0.5f);
    real32 bottom = -top;

    real32 left = -aspect * top;
    real32 right = -left;

    mat4 result;

    result.columns[0] = V4(2 * nearPlane / (left - right), 0.0f, 0.0f, 0.0f);
    result.columns[1] = V4(0.0f, 2 * nearPlane / (top - bottom), 0.0f, 0.0f);
    result.columns[2] = V4(0.0f, 0.0f, 1.0f, 1.0f);
    result.columns[3] = V4(0.0f, 0.0f, -2.0f * nearPlane, 0.0f);

    return result;
}

inline mat4 LookAt(vec3 camPos, vec3 pt, vec3 Y) {

    vec3 forward = Normalize(camPos - pt);
    vec3 right = Cross(Y, forward);
    vec3 up = Cross(forward, right);

    mat4 result = {};

#if 0
    result.columns[0][0] = right.x;
    result.columns[1][0] = right.y;
    result.columns[2][0] = right.z;

    result.columns[0][1] = up.x;
    result.columns[1][1] = up.y;
    result.columns[2][1] = up.z;

    result.columns[0][2] = forward.x;
    result.columns[1][2] = forward.y;
    result.columns[2][2] = forward.z;

    result.columns[0][3] = camPos.x;
    result.columns[1][3] = camPos.y;
    result.columns[2][3] = camPos.z;

#else
    result.columns[0][0] = right.x;
    result.columns[0][1] = right.y;
    result.columns[0][2] = right.z;

    result.columns[1][0] = up.x;
    result.columns[1][1] = up.y;
    result.columns[1][2] = up.z;

    result.columns[2][0] = forward.x;
    result.columns[2][1] = forward.y;
    result.columns[2][2] = forward.z;

    result.columns[3][0] = camPos.x;
    result.columns[3][1] = camPos.y;
    result.columns[3][2] = camPos.z;

    result.columns[3][3] = 1;
#endif
    return result;
}

#endif

