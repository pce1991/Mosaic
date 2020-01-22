
#ifndef _ETC_MATH_GEOMETRY_H
#define _ETC_MATH_GEOMETRY_H

// Coordinate Spaces
inline vec3 PointToLocalSpace(vec3 origin, quaternion rotation, vec3 scale, vec3 referenceSpacePoint) {

    vec3 X = Rotate(rotation, V3(scale.x, 0.0f, 0.0f));
    vec3 Y = Rotate(rotation, V3(0.0f, scale.y, 0.0f));
    vec3 Z = Rotate(rotation, V3(0.0f, 0.0f, scale.z));
    
    vec3 result;

    result.x = Dot(referenceSpacePoint - origin, X);
    result.y = Dot(referenceSpacePoint - origin, Y);
    result.z = Dot(referenceSpacePoint - origin, Z);

    return result;
}

inline vec3 PointToLocalSpace(vec3 origin, quaternion rotation, real32 scale, vec3 referenceSpacePoint) {
    vec3 result = PointToLocalSpace(origin, rotation, V3(scale, scale, scale), referenceSpacePoint);
    return result;
}

inline vec3 PointToLocalSpace(vec3 origin, quaternion rotation, vec3 referenceSpacePoint) {
    vec3 result = PointToLocalSpace(origin, rotation, V3(1, 1, 1), referenceSpacePoint);
    return result;
}

inline vec3 PointToReferenceSpace(vec3 origin, quaternion rotation, vec3 scale, vec3 localPoint) {
    vec3 X = Rotate(rotation, V3(scale.x, 0.0f, 0.0f));
    vec3 Y = Rotate(rotation, V3(0.0f, scale.y, 0.0f));
    vec3 Z = Rotate(rotation, V3(0.0f, 0.0f, scale.z));

    vec3 result = origin + X * localPoint.x + Y * localPoint.y + Z * localPoint.z;
    
    return result;
}

inline vec3 PointToReferenceSpace(vec3 origin, quaternion rotation, real32 scale, vec3 localPoint) {
    vec3 result = PointToReferenceSpace(origin, rotation, V3(scale, scale, scale), localPoint);
    return result;
}

inline vec3 PointToReferenceSpace(vec3 origin, quaternion rotation, vec3 localPoint) {
    vec3 result = PointToReferenceSpace(origin, rotation, V3(1, 1, 1), localPoint);
    return result;
}

inline vec3 DirectionToLocalSpace(quaternion rotation, vec3 referenceSpaceDirection) {

    vec3 X = Rotate(rotation, V3(1.0f, 0.0f, 0.0f));
    vec3 Y = Rotate(rotation, V3(0.0f, 1.0f, 0.0f));
    vec3 Z = Rotate(rotation, V3(0.0f, 0.0f, 1.0f));

    vec3 result;

    result.x = Dot(referenceSpaceDirection, X);
    result.y = Dot(referenceSpaceDirection, Y);
    result.z = Dot(referenceSpaceDirection, Z);

    result = Normalize(result);

    return result;
}

inline vec3 DirectionToReferenceSpace(quaternion rotation, vec3 localDirection) {
    vec3 result = Normalize(Rotate(rotation, localDirection));
    return result;
}

// Spherical coordinates are {azimuth, zenith, radius}, in radians
inline vec3 CartesianToSphericalCoordinates(vec3 cartesianPoint) {
    real32 r = Length(cartesianPoint);
    real32 zenith = acosf(cartesianPoint.y / r);
    real32 azimuth = atan2f(cartesianPoint.z, cartesianPoint.x);

    vec3 result = V3(azimuth, zenith, r);
    return result;
}

inline vec3 SphericalToCartesianCoordinates(vec3 sphericalPoint) {

    real32 r = sphericalPoint.z;
    real32 zenith = sphericalPoint.y;
    real32 azimuth = sphericalPoint.x;

    vec3 result;

    result.y = r * cosf(zenith);
    result.x = r * sinf(zenith) * cosf(azimuth);
    result.z = r * sinf(zenith) * sinf(azimuth);
    
    return result;
}

bool PointsAreCollinear(vec2 a, vec2 b, vec2 c, real32 epsilon = 0.0f) {
    return NearlyEquals((b.y - a.y) * (c.x - b.x), (c.y - b.y) * (b.x - a.x), 0.0f, epsilon);
}

bool PointsAreCollinear(vec3 a, vec3 b, vec3 c, real32 epsilon = 0.0f) {
    return NearlyEquals(Length(Cross(c - a, b - a)), 0.0f, epsilon);
}

struct Plane {
    // @TODO: get rid of this so we save space, and conceptually we're mathier
    // I havent seen anyone else define a plane by using a point, so I think its not useful
    vec3 point;
    
    vec3 normal;
    r32 d; // d = Dot(point, normal)
};

inline Plane TransformPlane(mat4 transform, Plane plane) {
    mat4 inverse;
    Inverse(transform, &inverse);

    Plane result = {};
    result.point = MultiplyPoint(transform, plane.point);

    vec4 v = LeftMultiply(V4(plane.normal, plane.d), inverse);
    result.normal = v.xyz;
    result.d = v.w;

    return result;
}

inline Plane MakePlane(vec3 pt, vec3 normal) {
    Plane p = {};
    p.point = pt;
    p.normal = normal;
    p.d = Dot(pt, normal);
    return p;
}

inline Plane MakePlane(vec3 pt, quaternion rotation) {
    Plane p = {};
    p.point = pt;
    p.normal = Rotate(rotation, FORWARD);
    return p;
}

// Lines
inline vec3 ClosestPointOnSegment(vec3 p, vec3 a, vec3 b, real32 *t) {
    vec3 result = {};
    vec3 ab = b - a;

    *t = Dot(p - a, ab);

    if (*t <= 0.0f) {
        *t = 0;
        result = a;
    }
    else {
        real32 denom = Dot(ab, ab);
        if (*t >= denom) {
            *t = 1.0f;
            result = b;
        }
        else {
            *t = *t / denom;
            result = a + (*t * ab);
        }
    }

    return result;
}

inline vec3 ClosestPointOnLine(vec3 p, vec3 a, vec3 b, real32 *t) {
    vec3 ab = b - a;

    *t = Dot(p - a, ab) / Dot(ab, ab);

    return a + (*t * ab);
}



// Computes squared distance between point C and segment AB
inline real32 SqDistancePointToSegment(vec3 a, vec3 b, vec3 c) {
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 bc = c - b;

    real32 e = Dot(ac, ab);
    // when c projects outside ab
    if (e <= 0.0f) {
        return Dot(ac, ac);
    }
    real32 f = Dot(ab, ab);

    if (e >= f) {
        return Dot(bc, bc);
    }

    return Dot(ac, ac) - e * e / f;
}

// computes closest points C1 and C2 of S1(s) = p1 + s * (q1 - p1) and S2(t) = p2 + t * (q2 - p2)
inline real32 ClosestPointSegmentToSegment(vec3 p1, vec3 q1, vec3 p2, vec3 q2,
                                           real32 *s, real32 *t, vec3 *c1, vec3 *c2) {
    // returns square distance

    vec3 d1 = q1 - p1;
    vec3 d2 = q2 - p2;
    vec3 r = p1 - p2;

    real32 a = Dot(d1, d1);
    real32 e = Dot(d2, d2);
    real32 f = Dot(d2, r);

    // check if segments degenerate to points.
    if (a <= FLT_EPSILON && e <= FLT_EPSILON) {
        *s = 0.0f;
        *t = 0.0f;
        *c1 = p1;
        *c2 = p2;

        return Dot(*c1 - *c2, *c1 - *c2);
    }
    else {
        real32 c = Dot(d1, r);
        if (e <= FLT_EPSILON) {
            *t = 0.0f;

            *s = Clamp(-c / 1, 0.0f, 1.0f);
        }
        else {
            real32 b = Dot(d1, d2);

            real32 denom = a * e - b * b;

            if (denom != 0.0f) {
                *s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else {
                *s = 0.0f;
            }

            *t = (b * *s + f) / e;

            if (*t < 0.0f) {
                *t = 0.0f;
                *s = Clamp(-c / a, 0.0f, 1.0f);
            }
            else if (*t > 1.0f) {
                *t = 1.0f;
                *s = Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    *c1 = p1 + d1 * *s;
    *c2 = p2 + d2 * *t;

    return Dot(*c1 - *c2, *c1 - *c2);
}

inline real32 DistanceFromLineSegment(vec3 p, vec3 p0, vec3 p1) {
    vec3 v = p1 - p0;
    vec3 w = p - p0;

    real32 c1 = Dot(w, v);
    if (c1 <= 0) {
        return Distance(p, p0);
    }

    real32 c2 = Dot(v, v);
    if (c2 <= c1) {
        return Distance(p, p1);
    }

    real32 b = c1 / c2;
    vec3 pb = p0 + (b * v);
    return Distance(p, pb);
}

inline real32 PerpProduct(vec2 a, vec2 b) {
    return a.y * b.x - a.x * b.y;
}

// @GAAAAAAAAAAAAAAAAAAAAAAAAACK
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
inline bool PointInSegment(vec2 a, vec2 b, vec2 p) {
    
    if (a.x != b.x) {
        if (a.x <= p.x && p.x <= b.x) {
            return true;
        }
        if (a.x >= p.x && p.x >= b.x) {
            return true;
        }
    }
    else {
        if (a.y <= p.y && p.y <= b.y) {
            return true;
        }
        if (a.y >= p.y && p.y >= b.y) {
            return true;
        }
    }
    return false;
}

inline bool PointInSegment(vec3 a, vec3 b, vec3 p) {
    
    if (a.x != b.x) {
        if (a.x <= p.x && p.x <= b.x) {
            return true;
        }
        if (a.x >= p.x && p.x >= b.x) {
            return true;
        }
    }
    else {
        if (a.y <= p.y && p.y <= b.y) {
            return true;
        }
        if (a.y >= p.y && p.y >= b.y) {
            return true;
        }
    }
    return false;
}

inline bool SegmentsOverlap(vec2 a, vec2 b, vec2 c, vec2 d) {
    bool seg1OverlapsSeg0 = (PointInSegment(a, b, c) || PointInSegment(a, b, d));
    bool seg0OverlapsSeg1 = (PointInSegment(c, d, a) || PointInSegment(c, d, b));

    bool seg1InSeg0 = (PointInSegment(a, b, c) && PointInSegment(a, b, d));
    bool seg0InSeg1 = (PointInSegment(c, d, a) && PointInSegment(c, d, b));

    return (seg1OverlapsSeg0 && seg0OverlapsSeg1) || (seg1InSeg0 || seg0InSeg1);
}

// @NOTE: sgements are ab, cd, t values relevant to segment ab
inline uint32 SegmentSegmentIntersection(vec2 a, vec2 b, vec2 c, vec2 d, vec2 *p0, vec2 *p1) {
    vec2 ab = b - a;
    vec2 cd = d - c;
    vec2 w = a - c;
    real32 D = PerpProduct(cd, ab);

    real32 epsilon = 1.0e-4;

    if (Abs(D) < epsilon) {
        if (PerpProduct(w, ab) != 0.0f || PerpProduct(w, cd) != 0.0f) {
            return 0;
        }

        real32 dab = Dot(ab, ab);
        real32 dcd = Dot(cd, cd);

        if (dab == 0.0f && dcd == 0.0f) {
            if (a != c) { return 0; }
            *p0 = a;
            return 1;
        }

        if (dab == 0.0f) {
            if (!PointInSegment(c, d, a)) { return 0; }
            *p0 = a;
            return 1;
        }
        if (dcd == 0.0f) {
            if (!PointInSegment(a, b, c)) { return 0; }
            *p0 = c;
            return 1;
        }

        real32 t0, t1;
        vec2 w2 = b - c;
        if (cd.x != 0.0f) {
            t0 = w.x / cd.x;
            t1 = w2.x / cd.x;
        }
        else {
            t0 = w.y / cd.y;
            t1 = w2.y / cd.y;
        }
        if (t0 > t1) { real32 temp = t0; t0 = t1; t1 = temp; }
        if (t0 > 1 || t1 < 0) { return 0; }
        t0 = Clamp(t0, 0.0f, 1.0f);
        t1 = Clamp(t1, 0.0f, 1.0f);

        if (t0 == t1) {
            *p0 = Lerp(c, d, t0);
            return 1;
        }

        *p0 = Lerp(c, d, t0);
        *p1 = Lerp(c, d, t1);
        return 2;
    }
    
    
    real32 si = PerpProduct(w, cd) / D;
    if (si < -epsilon || si > 1.0f + epsilon) { return 0; }
    real32 ti = PerpProduct(w, ab) / D;
    if (ti < -epsilon || ti > 1.0f + epsilon) { return 0; }
    
    *p0 = Lerp(a, b, si);
    return 1;
}
// @KAPLAH
// @GAAAAAAAAAAAAAAACK #END

inline bool LinePlaneIntersection(vec3 a, vec3 b, vec3 point, vec3 normal, real32 *t, vec3 *p, real32 epsilon = 0.0f) {
    vec3 ab = b - a;

    real32 d = Dot(normal, point);

    *t = (d - Dot(normal, a)) / Dot(normal, ab);
    
    //if (*t >= 0.0f - epsilon && *t <= 1.0f + epsilon) {
    if (-epsilon <= *t && *t <= 1.0f + epsilon) {
        if (p != NULL) {
            *p = a + (*t * ab);
        }
        
        return true;
    }

    return false;
}

inline bool LinePlaneIntersection(vec3 a, vec3 b, Plane plane, real32 *t, vec3 *p, real32 epsilon = 0.0f) {
    return LinePlaneIntersection(a, b, plane.point, plane.normal, t, p, epsilon);
}


inline real32 PerpendicularDistance(vec3 a, vec3 direction, vec3 p) {
    real32 d = Length(Cross(Normalize(direction), p - a));
    return d;
}

inline real32 PerpendicularDistance(vec2 a, vec2 direction, vec2 p) {
    return PerpendicularDistance(V3(a, 0.0f), V3(direction, 0.0f), V3(p, 0.0f));
}

// @TODO: doesnt test line inside sphere
inline bool SegmentSphereIntersection(vec3 p0, vec3 p1, vec3 center, r32 radius, r32 *t, vec3 *point) {
    vec3 diff = p1 - p0;
    r32 len = Length(diff);

    vec3 d = diff / len;
    
    vec3 m = p0 - center;
    r32 b = Dot(m, d);
    r32 c = Dot(m, m) - (radius * radius);

    if (c > 0.0f && b > 0.0f) { return false; }

    r32 discr = (b * b) - c;

    if (discr < 0.0f) { return false; }
    
    *t = -b - sqrt(discr);
    if (*t < 0.0f) { return false; }
    if (*t > len) { return false; }
    *point = p0 + (d * (*t));
    
    return true;
}

inline bool SegmentIntersectsTriangle(vec3 p, vec3 q, vec3 a, vec3 b, vec3 c, r32 *t, r32 *u, r32 *v, r32 *w) {

    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 qp = p - q;

    // @PERF: may want to cache this if checking many segments against a triangle
    vec3 n = Cross(ab, ac);

    r32 d = Dot(qp, n);

    if (d <= 0.0) { return false; }

    vec3 ap = p - a;
    *t = Dot(ap, n);

    if (*t < 0) { return false; }
    if (*t > d ) { return false; }

    vec3 e = Cross(qp, ap);
    *v = Dot(ac, e);

    if (*v < 0.0f || *v > d) { return false; }

    *w = -Dot(ab, e);
    if (*w < 0.0f || *v + *w > d) { return false; }

    r32 ood = 1.0f / d;
    *t *= ood;
    *v *= ood;
    *w *= ood;
    *u = 1.0f - *v - *w;
    
    return true;
}

// Douglas-Peucker Simplification
inline int32 SimplifyPolyline(vec2 *points, uint32 pointCount, vec2 *simplified, real32 threshold) {
    if (pointCount <= 2) {
        for (int i = 0; i < pointCount; i++) {
            simplified[i] = points[i];
        }

        return pointCount;
    }

    int32 simplifiedPointCount = 0;

    vec2 start = points[0];
    vec2 end = points[pointCount - 1];

    real32 maxDistance = threshold;
    int32 maxIndex = -1;

    for (int i = 1; i < pointCount - 1; i++) {
        real32 distance = PerpendicularDistance(start, end - start, points[i]);

        if (distance > maxDistance) {
            maxDistance = distance;
            maxIndex = i;
        }
    }

    if (maxIndex >= 0) {
        simplifiedPointCount += SimplifyPolyline(points, maxIndex + 1, simplified, threshold);

        // @NOTE: subtract one so the pivot point isn't counted twice
        simplifiedPointCount--;

        simplifiedPointCount += SimplifyPolyline(points + maxIndex, pointCount - maxIndex, simplified + simplifiedPointCount, threshold);
    }
    else {
        simplified[0] = start;
        simplified[1] = end;
        simplifiedPointCount = 2;
    }

    return simplifiedPointCount;
}

inline int32 SimplifyPolyline3D(vec3 *points, uint32 pointCount, vec3 *simplified, real32 threshold) {
    if (pointCount <= 2) {
        for (int i = 0; i < pointCount; i++) {
            simplified[i] = points[i];
        }

        return pointCount;
    }

    int32 simplifiedPointCount = 0;

    vec3 start = points[0];
    vec3 end = points[pointCount - 1];

    real32 maxDistance = threshold;
    int32 maxIndex = -1;

    for (int i = 1; i < pointCount - 1; i++) {
        real32 distance = PerpendicularDistance(start, end - start, points[i]);

        if (distance > maxDistance) {
            maxDistance = distance;
            maxIndex = i;
        }
    }

    if (maxIndex >= 0) {
        simplifiedPointCount += SimplifyPolyline3D(points, maxIndex + 1, simplified, threshold);

        // @NOTE: subtract one so the pivot point isn't counted twice
        simplifiedPointCount--;

        simplifiedPointCount += SimplifyPolyline3D(points + maxIndex, pointCount - maxIndex, simplified + simplifiedPointCount, threshold);
    }
    else {
        simplified[0] = start;
        simplified[1] = end;
        simplifiedPointCount = 2;
    }

    return simplifiedPointCount;
}

// Clips segment AB to plane defined by P and Normal. Returns true if
// any of the segment lies on the positive side of the plane
inline bool ClipHomogeneousSegment(vec4 a0, vec4 b0, vec4 p, vec4 normal, vec4 *a1, vec4 *b1) {
    bool insidePlane;

    real32 dA = Dot(a0 - p, normal);
    real32 dB = Dot(b0 - p, normal);

    // 1) Both points are on negative-side
    if (dA < 0.0f && dB < 0.0f) {
        insidePlane = false;
    }
    // 2) Both points are on positive-side
    else if (dA >= 0.0f && dB >= 0.0f) {
        *a1 = a0;
        *b1 = b0;
        insidePlane = true;
    }
    // 3) One point on positive-side, so clip against plane
    else {
        real32 t = dA / (dA - dB);
        vec4 intersection = a0 + t * (b0 - a0);

        if (dA >= 0.0f) {
            *a1 = a0;
            *b1 = intersection;
        }
        else {
            *a1 = intersection;
            *b1 = b0;
        }

        insidePlane = true;
    }

    return insidePlane;
}

#define W_CLIPPING_PLANE 0.00001f

// Clips segment AB against w=-x, w=x, w=-y, w=y, w=-z, w=z, and
// w=W_CLIPPING_PLANE. Returns true if any of the segment lies within
// the region
inline bool ClipHomogeneousSegmentToView(vec4 a0, vec4 b0, vec4 *a1, vec4 *b1) {
    *a1 = a0;
    *b1 = b0;

    static vec4 p = V4(0, 0, 0, 0);
    static vec4 normalNegX = Normalize(V4(1, 0, 0, 1));
    static vec4 normalPosX = Normalize(V4(-1, 0, 0, 1));
    static vec4 normalNegY = Normalize(V4(0, 1, 0, 1));
    static vec4 normalPosY = Normalize(V4(0, -1, 0, 1));
    static vec4 normalNegZ = Normalize(V4(0, 0, 1, 1));
    static vec4 normalPosZ = Normalize(V4(0, 0, -1, 1));

    static vec4 wClipPoint = V4(0.0f, 0.0f, 0.0f, W_CLIPPING_PLANE);
    static vec4 wClipNormal = V4(0, 0, 0, 1);

    bool insideView = (ClipHomogeneousSegment(*a1, *b1, p, normalNegX, a1, b1) &&
                      ClipHomogeneousSegment(*a1, *b1, p, normalPosX, a1, b1) &&
                      ClipHomogeneousSegment(*a1, *b1, p, normalNegY, a1, b1) &&
                      ClipHomogeneousSegment(*a1, *b1, p, normalPosY, a1, b1) &&
                      ClipHomogeneousSegment(*a1, *b1, p, normalNegZ, a1, b1) &&
                      ClipHomogeneousSegment(*a1, *b1, p, normalPosZ, a1, b1) &&
                      ClipHomogeneousSegment(*a1, *b1, wClipPoint, wClipNormal, a1, b1));

    return insideView;
}

inline bool HomogeneousSegmentInPositiveHalfPlane(vec4 a, vec4 b, vec4 p, vec4 normal) {
    real32 dA = Dot(a - p, normal);
    real32 dB = Dot(b - p, normal);

    if (dA < 0.0f && dB < 0.0f) {
        return false;
    }

    return true;
}

inline bool HomogeneousSegmentInView(vec4 a, vec4 b) {
    static vec4 p = V4(0, 0, 0, 0);
    static vec4 normalNegX = Normalize(V4(1, 0, 0, 1));
    static vec4 normalPosX = Normalize(V4(-1, 0, 0, 1));
    static vec4 normalNegY = Normalize(V4(0, 1, 0, 1));
    static vec4 normalPosY = Normalize(V4(0, -1, 0, 1));
    static vec4 normalNegZ = Normalize(V4(0, 0, 1, 1));
    static vec4 normalPosZ = Normalize(V4(0, 0, -1, 1));

    static vec4 wClipPoint = V4(0.0f, 0.0f, 0.0f, W_CLIPPING_PLANE);
    static vec4 wClipNormal = V4(0, 0, 0, 1);

    bool insideView = (HomogeneousSegmentInPositiveHalfPlane(a, b, p, normalNegX) &&
                      HomogeneousSegmentInPositiveHalfPlane(a, b, p, normalPosX) &&
                      HomogeneousSegmentInPositiveHalfPlane(a, b, p, normalNegY) &&
                      HomogeneousSegmentInPositiveHalfPlane(a, b, p, normalPosY) &&
                      HomogeneousSegmentInPositiveHalfPlane(a, b, p, normalNegZ) &&
                      HomogeneousSegmentInPositiveHalfPlane(a, b, p, normalPosZ) &&
                      HomogeneousSegmentInPositiveHalfPlane(a, b, wClipPoint, wClipNormal));

    return insideView;
}


// Triangles
// @NOTE: assumes triangle is wound counter clockwise
inline bool PointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) {
    if (Cross(point - a, b - a) < 0) return false;
    if (Cross(point - b, c - b) < 0) return false;
    if (Cross(point - c, a - c) < 0) return false;

    return true;
}

// [0]: Realtime Collision Detection, pg 152
// Returns 2 * the signed triangle area. The result is positive if abc
// is ccw, negative if abc is cw, and 0 if degenerate.
inline real32 SignedTriangleArea(vec2 a, vec2 b, vec2 c) {
    return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}

inline vec3 CartesianToBarycentric(vec3 a, vec3 b, vec3 c, vec3 point) {
    vec3 v0 = b - a;
    vec3 v1 = c - a;
    vec3 v2 = point - a;

    real32 d00 = Dot(v0, v0);
    real32 d01 = Dot(v0, v1);
    real32 d11 = Dot(v1, v1);
    real32 d20 = Dot(v2, v0);
    real32 d21 = Dot(v2, v1);
    real32 denom = d00 * d11 - d01 * d01;

    real32 v = (d11 * d20 - d01 * d21) / denom;
    real32 w = (d00 * d21 - d01 * d20) / denom;
    real32 u = 1.0f - v - w;

    vec3 result = V3(u, v, w);
    
    return result;
}

inline vec3 CartesianToBarycentric(vec2 a, vec2 b, vec2 c, vec2 point) {
    return CartesianToBarycentric(V3(a, 0.0f), V3(b, 0.0f), V3(c, 0.0f), V3(point, 0.0f));
}

inline vec3 BarycentricToCartesian(vec3 a, vec3 b, vec3 c, vec3 point) {
    vec3 result = a * point.x + b * point.y + c * point.z;
    return result;
}

inline vec2 BarycentricToCartesian(vec2 a, vec2 b, vec2 c, vec3 point) {
    vec2 result = a * point.x + b * point.y + c * point.z;
    return result;
}

inline bool PointInTriangle(vec3 a, vec3 b, vec3 c, vec3 p) {
    a = a - p;
    b = b - p;
    c = c - p;

    vec3 u = Cross(b, c);
    vec3 v = Cross(c, a);

    if (Dot(u, v) < 0.0f) { return false; }

    vec3 w = Cross(a, b);

    if (Dot(u, w) < 0.0f) { return false; }

    return true;
}

// [0]: Realtime Collision Detection, pg 141
inline vec3 ClosestPointOnTriangle(vec3 a, vec3 b, vec3 c, vec3 point) {
    // Check if point in vertex region outside A
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 ap = point - a;

    real32 d1 = Dot(ab, ap);
    real32 d2 = Dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;  // barycentric coordinates (1, 0, 0)

    // Check if point in vertex region outside B
    vec3 bp = point - b;
    real32 d3 = Dot(ab, bp);
    real32 d4 = Dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;  // barycentric coordinates (0, 1, 0)

    // Check if point in edge region of AB, if so return projection of point onto AB
    real32 vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        real32 v = d1 / (d1 - d3);
        return a + v * ab;  // barycentric coordinates (1-v, v, 0)
    }

    // Check if point in vertex region outside C
    vec3 cp = point - c;
    real32 d5 = Dot(ab, cp);
    real32 d6 = Dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;  // barycentric coordinates (0, 0, 1)

    // Check if point in edge regions of AC, if so return projection of point onto AC
    real32 vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        real32 w = d2 / (d2 - d6);
        return a + w * ac;  // barycentric coordinates (1-w, 0, w)
    }

    // Check if point in edge region of BC, if so return projection of point onto BC
    real32 va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        real32 w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b);
    }

    // point inside face region. Compute Q through its barycentric coordintaes (u, v, w)
    real32 denom = 1.0f / (va + vb + vc);
    real32 v = vb * denom;
    real32 w = vc * denom;
    
    return a + ab * v + ac * w;
}

inline vec2 ClosestPointOnTriangle(vec2 a, vec2 b, vec2 c, vec2 point) {
    return ClosestPointOnTriangle(V3(a, 0.0f), V3(b, 0.0f), V3(c, 0.0f), V3(point, 0.0f)).xy;
}


bool PointOutsideOfPlane(vec3 p, vec3 a, vec3 b, vec3 c) {
    return Dot(p - a, Cross(b - a, c - a)) >= 0;
}

// @NOTE: take 4 points since we may not know the winding order
bool PointOutsideOfPlane(vec3 p, vec3 a, vec3 b, vec3 c, vec3 d) {
    real32 signP =  Dot(p - a, Cross(b - a, c - a));
    real32 signD =  Dot(d - a, Cross(b - a, c - a));
    return signP * signD < 0.0f;
}

vec3 ClosestPointOnTetrahedron(vec3 p, vec3 a, vec3 b, vec3 c, vec3 d) {
    vec3 closest = p;
    real32 bestSqDist = FLT_MAX;

    if (PointOutsideOfPlane(p, a, b, c, d)) {
        vec3 q = ClosestPointOnTriangle(a, b, c, p);
        real32 sqDist = Dot(q - p, q - p);
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            closest = q;
        }
    }

    if (PointOutsideOfPlane(p, a, c, d, b)) {
        vec3 q = ClosestPointOnTriangle(a, c, d, p);
        real32 sqDist = Dot(q - p, q - p);
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            closest = q;
        }
    }

    if (PointOutsideOfPlane(p, a, d, b, c)) {
        vec3 q = ClosestPointOnTriangle(a, d, b, p);
        real32 sqDist = Dot(q - p, q - p);
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            closest = q;
        }
    }

    if (PointOutsideOfPlane(p, b, d, c, a)) {
        vec3 q = ClosestPointOnTriangle(b, d, c, p);
        real32 sqDist = Dot(q - p, q - p);
        if (sqDist < bestSqDist) {
            bestSqDist = sqDist;
            closest = q;
        }
    }

    return closest;
}


// Rectangles
struct EtcRect {
    vec2 pos;
    vec2 size;
};

struct EtcRecti {
    vec2i pos;
    vec2i size;
};

inline EtcRect RectFromAABB(vec2 min, vec2 max) {
    EtcRect result;
    result.pos = min;
    result.size = max - min;

    return result;
}

inline EtcRect EnclosingAABB(vec2 *points, uint32 pointCount) {
    real32 minX = INFINITY;
    real32 maxX = -INFINITY;
    real32 minY = INFINITY;
    real32 maxY = -INFINITY;

    for (int i = 0; i < pointCount; i++) {
        vec2 point = points[i];

        minX = Min(minX, point.x);
        maxX = Max(maxX, point.x);

        minY = Min(minY, point.y);
        maxY = Max(maxY, point.y);
    }

    struct EtcRect result;
    result.pos = V2(minX, minY);
    result.size = V2(maxX - minX, maxY - minY);

    return result;
}

inline EtcRecti Intersection(EtcRecti a, EtcRecti b) {
    EtcRecti result;

    result.pos.x = Max(a.pos.x, b.pos.x);
    result.pos.y = Max(a.pos.y, b.pos.y);

    int32 right = Min(a.pos.x + a.size.x, b.pos.x + b.size.x);
    int32 bottom = Min(a.pos.y + a.size.y, b.pos.y + b.size.y);

    result.size.x = Max(right - result.pos.x, 0);
    result.size.y = Max(bottom - result.pos.y, 0);

    return result;
}

inline EtcRect Union(EtcRect a, EtcRect b) {
    EtcRect result;

    result.pos = Min(a.pos, b.pos);

    real32 right = Max(a.pos.x + a.size.x, b.pos.x + b.size.x);
    real32 bottom = Max(a.pos.y + a.size.y, b.pos.y + b.size.y);

    result.size.x = right - result.pos.x;
    result.size.y = bottom - result.pos.y;
    
    return result;
}

inline bool PointInRect(EtcRect a, vec2 point) {
    bool result = (point.x >= a.pos.x && point.x < (a.pos.x + a.size.x) &&
                  point.y >= a.pos.y && point.y < (a.pos.y + a.size.y));

    return result;
}

inline bool PointInRect(EtcRecti a, vec2 point) {
    bool result = (point.x >= a.pos.x && point.x < (a.pos.x + a.size.x) &&
                  point.y >= a.pos.y && point.y < (a.pos.y + a.size.y));

    return result;
}


// Planes
// @GACK: the order of arguments is not consistent here

inline vec3 ClosestPointOnPlane(vec3 point, vec3 planePoint, vec3 normal) {
    real32 d = Dot(normal, planePoint);
    real32 t = Dot(normal, point) - d;

    return point - (t * normal);
}


inline vec3 ClosestPointOnPlane(vec3 point, vec3 normal, real32 d) {
    real32 t = Dot(normal, point) - d;
    return point - (t * normal);
}

inline vec3 ClosestPointOnPlane(vec3 point, Plane plane) {
    return ClosestPointOnPlane(point, plane.point, plane.normal);
}

inline real32 PlaneTest(vec3 point, vec3 normal, vec3 testPoint) {
    real32 result = Dot(testPoint - point, normal);
    return result;
}

inline real32 PlaneTest(Plane plane, vec3 testPoint) {
    return PlaneTest(plane.point, plane.normal, testPoint);
}

inline real32 PlaneDistance(vec3 point, vec3 normal, vec3 planePoint) {
    real32 d = Dot(normal, planePoint);
    real32 d_ = Dot(normal, point) - d;
    return (d_ / Dot(normal, normal));
}

inline real32 PlaneDistance(vec3 point, Plane plane) {
    return PlaneDistance(point, plane.normal, plane.point);
}

inline bool LineLiesOnPlane(vec3 a, vec3 b, Plane plane, r32 epsilon = 0.0f) {
    r32 distA = PlaneDistance(a, plane);
    r32 distB = PlaneDistance(b, plane);

    return (distA > -epsilon && distA < epsilon &&
            distB > -epsilon && distB < epsilon);
}

inline bool TestPointFrustum(vec3 p, Plane *planes, int32 planeCount, r32 *results) {
    bool success = true;

    r32 r = 0;
    for (int i = 0; i < planeCount; i++) {
        r = PlaneTest(planes[i], p);
        if (results != NULL) {
            results[i] = r;
        }
        if (r < 0) {
            success = false;
        }
    }

    return success;
}

inline bool PlanePlaneIntersection(Plane a, Plane b, vec3 *pt, vec3 *dir, r32 eps = 0.0f) {
    vec3 u = Cross(a.normal, b.normal);

    r32 denom = Dot(u, u);
    if (denom <= eps) { return false; }

    *dir = u;
    *pt = Cross((a.d * b.normal) - (b.d * a.normal), u) / denom;

    return true;
}

inline bool PlanePlanePlaneIntersection(Plane a, Plane b, Plane c, vec3 *pt, real32 eps = 0.0f) {

    real32 d = Dot(a.normal, Cross(b.normal, c.normal));
    if (d < -eps || d > eps) {
        *pt = (-a.d * Cross(b.normal, c.normal) - (b.d * Cross(c.normal, a.normal)) - (c.d * Cross(a.normal, b.normal))) / d;
        return true;
    }
    return false;
}

inline bool PlaneSegmentIntersection(Plane p, vec3 a, vec3 b, vec3 *pt) {
    vec3 ab = b - a;

    r32 t = (p.d - Dot(p.normal, a)) / Dot(p.normal, ab);

    if (t >= 0 && t <= 1.0f) {
        *pt = a + t * ab;
        return true;
    }

    return false;
}

// Shamelessly stolen from: http://box2d.org/2014/02/computing-a-basis/
inline void ConstructBasis(vec3 normal, vec3 *X, vec3 *Y) {
    // Suppose normal vector has all equal components and is a unit vector: normal = (s, s, s)
    // Then 3*s*s = 1, s = sqrt(1/3) = 0.57735. This means that at least one component of a
    // unit vector must be greater or equal to 0.57735.
 
    if (Abs(normal.x) >= 0.57735f) {
        *Y = Normalize(V3(normal.y, -normal.x, 0.0f));
    }
    else {
        *Y = Normalize(V3(0.0f, normal.z, -normal.y));
    }
 
    *X = Cross(normal, *Y);
}

inline vec2 ProjectTo2D(vec3 origin, vec3 X, vec3 Y, vec3 pointToProject) {
    vec2 result;

    result.x = Dot(X, pointToProject - origin);
    result.y = Dot(Y, pointToProject - origin);

    return result;
}

inline vec2 ProjectTo2D(vec3 normal, vec3 point) {
    uint32 xIndex = 0;
    uint32 yIndex = 1;

    vec3 absNormal = V3(Abs(normal[0]), Abs(normal[1]), Abs(normal[2]));

    if (absNormal[0] >= absNormal[1] &&
        absNormal[0] >= absNormal[2]) {
        xIndex = 1;
        yIndex = 2;
    }
    else if (absNormal[1] >= absNormal[0] &&
             absNormal[1] >= absNormal[2]) {
        xIndex = 2;
        yIndex = 0;
    }
    else if (absNormal[2] >= absNormal[0] &&
             absNormal[2] >= absNormal[1]) {
        xIndex = 0;
        yIndex = 1;
    }

    return V2(point.data[xIndex], point.data[yIndex]);
}

inline vec3 ProjectTo3D(vec2 point, vec3 origin, vec3 X, vec3 Y) {
    vec3 result = {};
    result.x = origin.x + (point.x * X.x) + (point.y * Y.x);
    result.y = origin.y + (point.x * X.y) + (point.y * Y.y);
    result.z = origin.z + (point.x * X.z) + (point.y * Y.z);

    return result;
}


// Polygons
inline bool PointInPolygon(vec2 *points, uint32 pointCount, vec2 point) {
    vec2 scanline = V2(1, 0);
    vec2 scanlinePerp = V2(0, 1);

    int intersections = 0;

    for (int i = 0; i < pointCount; i++) {
        vec2 a = points[i];
        vec2 b = points[(i + 1) % pointCount];

        vec2 u = b - a;
        if (Dot(u, scanlinePerp) == 0) {
            continue;
        }

        vec2 uPerp = V2(-u.y, u.x);
        vec2 w = a - point;

        real32 s = -Dot(scanlinePerp, w) / Dot(scanlinePerp, u);
        real32 t = Dot(uPerp, w) / Dot(uPerp, scanline);

        if (0 < s && s <= 1 && t >= 0) {
            intersections++;
        }
    }

    bool pointInPolygon = (intersections % 2) == 1;
    return pointInPolygon;
}


enum PolygonFeatureType : uint8 {
    PolygonFeature_Vert,
    PolygonFeature_Edge,
    PolygonFeature_Face,
};

// @NOTE: We check for Collinearity of points, otherwise this wont work for polygons with collinear points
// Because it will potentially count one line an even number of times
// @WARNING: This will fail if any 3 points are collinear!!!!
inline bool PointInPolygon(vec3 normal, vec3 *points, uint32 pointCount, uint32 pointStride, uint32 *indices, vec3 point) {

    if (pointCount < 3) {
        return false;
    }

    if (pointStride == 0) {
        pointStride = sizeof(vec3);
    }
        
    vec3 X, Y;
    ConstructBasis(normal, &X, &Y);

    vec2 scanline = V2(1, 0);
    vec2 scanlinePerp = V2(0, 1);

    vec3 origin = STRIDED_READ(vec3, points, pointStride, 0);
    if (indices != NULL) {
        origin = STRIDED_READ(vec3, points, pointStride, indices[0]);
    }
    
    vec2 point2D = ProjectTo2D(origin, X, Y, point);
    
    int intersections = 0;

    for (int i = 0; i < pointCount; i++) {
        vec2 a;
        vec2 b;
        if (indices != NULL) {
            a = ProjectTo2D(origin, X, Y, STRIDED_READ(vec3, points, pointStride, indices[i]));
            b = ProjectTo2D(origin, X, Y, STRIDED_READ(vec3, points, pointStride, indices[(i + 1) % pointCount]));
        }
        else {
            a = ProjectTo2D(origin, X, Y, STRIDED_READ(vec3, points, pointStride, i));
            b = ProjectTo2D(origin, X, Y, STRIDED_READ(vec3, points, pointStride, (i + 1) % pointCount));
        }

        vec2 u = b - a;
        if (Dot(u, scanlinePerp) == 0) {
            continue;
        }

        vec2 uPerp = V2(-u.y, u.x);
        vec2 w = a - point2D;

        real32 s = -Dot(scanlinePerp, w) / Dot(scanlinePerp, u);
        real32 t = Dot(uPerp, w) / Dot(uPerp, scanline);

        // @MAYBE @BUG: this use to check 0 <= s but it failed in some cases.
        // Imagine a trapezoid:
        //  ---------
        // \         |
        //  \ *      |
        //   \       |
        //     ------
        // This point would fail since the ray would intersect the left side, and then at s == 0
        // on the bottom side, yielding an intersection count of 2
        if (0 < s && s <= 1 && t >= 0) {
            intersections++;
        }
    }

    bool pointInPolygon = (intersections % 2) == 1;
    return pointInPolygon;
}

struct ClosestPointOnPolygonQuery {
    PolygonFeatureType type;
    int32 index;
};

inline vec3 ClosestPointOnPolygon(vec3 point, vec3 normal, vec3 *points, u32 pointCount, ClosestPointOnPolygonQuery *query) {
    Plane plane; plane.point = points[0]; plane.normal = normal;
    
    vec3 closestPointOnPlane = ClosestPointOnPlane(point, plane);

    if (PointInPolygon(normal, points, pointCount, 0, NULL, point)) {
        if (query != NULL) {
            query->type = PolygonFeature_Face;
            query->index = -1;
        }
        return closestPointOnPlane;
    }

    vec3 closestPoint = point;
    
    real32 minDist = INFINITY;
    for (int i = 0; i < pointCount; i++) {
        vec3 a = points[i];
        vec3 b = points[(i + 1) % pointCount];

        real32 t;
        vec3 p = ClosestPointOnSegment(point, a, b, &t);

        real32 dist = Distance(point, p);
        if (dist < minDist) {
            minDist = dist;
            closestPoint = p;

            if (query != NULL) {
                if (t == 0.0f) {
                    query->type = PolygonFeature_Vert;
                    query->index = i;
                }
                else if (t == 1.0f) {
                    query->type = PolygonFeature_Vert;
                    query->index = (i + 1) % pointCount;
                }
                else {
                    query->type = PolygonFeature_Edge;
                    query->index = i;
                }
            }
        }
    }

    return closestPoint;
}

inline vec3 ClosestPointOnPolygon(vec3 point, vec3 normal, vec3 *points, u32 pointCount) {
    return ClosestPointOnPolygon(point, normal, points, pointCount, NULL);
}

inline vec2 Centroid(vec2 *points, uint32 pointCount) {
    vec2 centroid = V2(0, 0);

    for (int i = 0; i < pointCount; i++) {
        centroid = centroid + points[i];
    }

    if (pointCount > 0) {
        centroid = centroid / pointCount;
    }

    return centroid;
}

inline vec3 Centroid(vec3 *points, uint32 pointCount, uint32 stride = 0, uint32 *indices = NULL) {

    if (stride == 0) {
        stride = sizeof(vec3);
    }

    vec3 centroid = V3(0, 0, 0);

    for (int i = 0; i < pointCount; i++) {
        centroid = centroid + STRIDED_READ(vec3, points, stride, indices != NULL ? indices[i] : i);
    }

    if (pointCount > 0) {
        centroid = centroid / pointCount;
    }

    return centroid;
}

inline real32 SignedAreaOfPolygon(vec2 *points, uint32 pointCount) {
    r32 result = 0.0f;
    for (int i = 0; i < pointCount; i++) {
        vec2 p0 = points[i];
        vec2 p1 = points[(i + 1) % pointCount];        
        result += (p0.x * p1.y - p1.x * p0.y);
    }

    return 0.5 * result;
}

inline vec2 CentroidOfPolygon(vec2 *points, uint32 pointCount) {
    r32 area = SignedAreaOfPolygon(points, pointCount);

    vec2 result = {};

    for (int i = 0; i < pointCount; i++) {
        vec2 p0 = points[i];
        vec2 p1 = points[(i + 1) % pointCount];

        result.x += (p0.x + p1.x) * (p0.x * p1.y - p1.x * p0.y);
        result.y += (p0.y + p1.y) * (p0.x * p1.y - p1.x * p0.y);
    }

    result = result * (1 / (6 * area));

    return result;
}

// returns positive when points are clockwise
inline real32 ShoelaceFormula(vec2 *points, uint32 pointCount, uint32 stride = 0) {
    if (stride == 0) {
        stride = sizeof(vec2);
    }

    real32 sum = 0.0f;

    for (int i = 0; i < pointCount; i++) {
        vec2 a = STRIDED_READ(vec2, points, stride, i);
        vec2 b = STRIDED_READ(vec2, points, stride, (i + 1) % pointCount);
        vec2 c = STRIDED_READ(vec2, points, stride, (i + 2) % pointCount);

        sum += b.x * (c.y - a.y);
    }

    real32 result = 0.5f * sum;
    return result;
}


inline real32 ShoelaceFormulaXZ(vec3 *points, uint32 pointCount, uint32 stride = 0) {
    if (stride == 0) {
        stride = sizeof(vec3);
    }

    real32 sum = 0.0f;

    for (int i = 0; i < pointCount; i++) {
        vec3 a = STRIDED_READ(vec3, points, stride, i);
        vec3 b = STRIDED_READ(vec3, points, stride, (i + 1) % pointCount);
        vec3 c = STRIDED_READ(vec3, points, stride, (i + 2) % pointCount);

        sum += b.x * (c.z - a.z);
    }

    real32 result = 0.5f * sum;
    return result;
}


// Rays
struct Ray {
    vec3 origin;
    vec3 direction;
};

inline Ray MakeRay(vec3 origin, vec3 direction) {
    Ray result;
    result.origin = origin;
    result.direction = direction;

    return result;
}

inline Ray TransformRay(mat4 transform, Ray ray) {
    Ray result;
    result.origin = MultiplyPoint(transform, ray.origin);
    result.direction = Normalize(MultiplyDirection(transform, ray.direction));
    return result;
}


inline vec3 PointAt(Ray ray, real32 t) {
    vec3 result = ray.origin + t * ray.direction;
    return result;
}

// Finds the parameter of Ray a closest to Ray b. Potentially finds
// points "behind" either ray.
// [0] http://geomalgorithms.com/a07-_distance.html
inline real32 ClosestToRayAt(Ray a, Ray b) {
    real32 s = 0;

    real32 a_ = Dot(a.direction, a.direction);
    real32 b_ = Dot(a.direction, b.direction);
    real32 c_ = Dot(b.direction, b.direction);
    real32 d_ = Dot(a.direction, a.origin - b.origin);
    real32 e_ = Dot(b.direction, a.origin - b.origin);

    real32 denom = a_ * c_ - Square(b_);
    if (denom != 0) {
        s = (b_ * e_ - c_ * d_) / denom;
    }

    return s;
}

// @TODO: I don't trust this at all -hz
inline real32 ClosestToPointAt(Ray ray, vec3 point) {
    real32 t = Dot(ray.direction, point - ray.origin);
    return t;
}

inline bool RaycastPlane(vec3 point, vec3 normal, Ray ray, real32 *t = NULL) {
    bool result = false;
    real32 intersection = 0.0f;

    real32 nd = Dot(ray.direction, normal);
    if (!NearlyEquals(nd, 0.0f)) {
        intersection = Dot(point - ray.origin, normal) / nd;
        result = intersection >= 0.0f;
    }

    if (t != NULL) {
        *t = intersection;
    }
    return result;
}

inline bool RaycastPlane(Plane plane, Ray ray, real32 *t = NULL) {
    return (bool)RaycastPlane(plane.point, plane.normal, ray, t);
}

inline bool IsTriangleDegenerate(vec3 a, vec3 b, vec3 c) {
    real32 parallelogramArea = Length(Cross(b - a, c - a));
    return NearlyEquals(parallelogramArea, 0.0f);
}

inline bool RaycastTriangle(vec3 a, vec3 b, vec3 c, Ray ray, real32 *t = NULL) {
    bool result = false;

    if (IsTriangleDegenerate(a, b, c)) {
        return false;
    }

    vec3 v0 = b - a;
    vec3 v1 = c - a;
        
    vec3 normal = Normalize(Cross(v0, v1));

    real32 planeT;
    if (RaycastPlane(a, normal, ray, &planeT)) {
        vec3 point = PointAt(ray, planeT);

        // Compute barycentric coordinates of point with respect to ABC
        vec3 v2 = point - a;

        real32 d00 = Dot(v0, v0);
        real32 d01 = Dot(v0, v1);
        real32 d11 = Dot(v1, v1);
        real32 d20 = Dot(v2, v0);
        real32 d21 = Dot(v2, v1);
        real32 denom = d00 * d11 - d01 * d01;

        real32 v = (d11 * d20 - d01 * d21) / denom;
        real32 w = (d00 * d21 - d01 * d20) / denom;
        real32 u = 1.0f - v - w;

        // Raycast succeeded if point is inside triangle
        result = u >= 0.0f && v >= 0.0f && w >= 0.0f;

        if (t != NULL) {
            *t = planeT;
        }
    }
    
    return result;
}

inline bool RaycastMesh(vec3 *points, uint32 *indices, uint32 indexCount, Ray ray, real32 *t = NULL) {
    ASSERT(indexCount % 3 == 0);

    bool result = false;

    uint32 triangleCount = indexCount / 3;
    real32 closestT = INFINITY;

    for (int i = 0; i < triangleCount; i++) {
        uint32 baseIndex = i * 3;

        vec3 a = points[indices[baseIndex + 0]];
        vec3 b = points[indices[baseIndex + 1]];
        vec3 c = points[indices[baseIndex + 2]];

        real32 thisT;
        if (RaycastTriangle(a, b, c, ray, &thisT)) {
            if (thisT < closestT) {
                closestT = thisT;
                result = true;
            }
        }
    }

    if (result && t != NULL) {
        *t = closestT;
    }

    return result;
}

inline bool RaycastMeshTriangleStrip(vec3 *points, uint32 count, Ray ray, real32 *t = NULL) {
    ASSERT(count >= 3);

    bool result = false;

    real32 closestT = INFINITY;

    vec3 a = points[0];
    vec3 b = points[1];
    vec3 c;
    int32 index = 2;

    while (index < count) {
        c = points[index];

        real32 thisT;
        if (RaycastTriangle(a, b, c, ray, &thisT)) {
            if (thisT < closestT) {
                closestT = thisT;
                result = true;
            }
        }

        a = b;
        b = c;
        index++;
    }

    if (result && t != NULL) {
        *t = closestT;
    }
    
    return result;
}

bool RaycastCube(vec3 center, vec3 halfExtents, Ray ray, real32 *t = NULL) {
    bool result = false;

    // @PERF: be smarter about exiting early

    r32 closestIntersection = INFINITY;
    r32 intersection;
    if (RaycastPlane(center + V3(0, 0, halfExtents.z), V3(0, 0, 1), ray, &intersection)) {
        vec3 point = PointAt(ray, intersection);

        if (point.x >= center.x - halfExtents.x && point.x <= center.x + halfExtents.x &&
            point.y >= center.y - halfExtents.y && point.y <= center.y + halfExtents.y) {

            if (intersection < closestIntersection) {
                closestIntersection = intersection;
            }
            
            result = true;
        }
    }

    if (RaycastPlane(center + V3(0, 0, -halfExtents.z), V3(0, 0, -1), ray, &intersection)) {
        vec3 point = PointAt(ray, intersection);

        if (point.x >= center.x - halfExtents.x && point.x <= center.x + halfExtents.x &&
            point.y >= center.y - halfExtents.y && point.y <= center.y + halfExtents.y) {
            
            if (intersection < closestIntersection) {
                closestIntersection = intersection;
            }
            
            result = true;
        }
    }

    if (RaycastPlane(center + V3(halfExtents.x, 0, 0), V3(1, 0, 0), ray, &intersection)) {
        vec3 point = PointAt(ray, intersection);

        if (point.z >= center.z - halfExtents.z && point.z <= center.z + halfExtents.z &&
            point.y >= center.y - halfExtents.y && point.y <= center.y + halfExtents.y) {
                        
            if (intersection < closestIntersection) {
                closestIntersection = intersection;
            }

            result = true;
        }
    }

    if (RaycastPlane(center + V3(-halfExtents.x, 0, 0), V3(-1, 0, 0), ray, &intersection)) {
        vec3 point = PointAt(ray, intersection);

        if (point.z >= center.z - halfExtents.z && point.z <= center.z + halfExtents.z &&
            point.y >= center.y - halfExtents.y && point.y <= center.y + halfExtents.y) {
                        
            if (intersection < closestIntersection) {
                closestIntersection = intersection;
            }

            result = true;
        }
    }

    if (RaycastPlane(center + V3(0, halfExtents.y, 0), V3(0, 1, 0), ray, &intersection)) {
        vec3 point = PointAt(ray, intersection);

        if (point.z >= center.z - halfExtents.z && point.z <= center.z + halfExtents.z &&
            point.x >= center.x - halfExtents.x && point.x <= center.x + halfExtents.x) {
                        
            if (intersection < closestIntersection) {
                closestIntersection = intersection;
            }

            result = true;
        }
    }

    if (RaycastPlane(center + V3(0, -halfExtents.y, 0), V3(0, -1, 0), ray, &intersection)) {
        vec3 point = PointAt(ray, intersection);

        if (point.z >= center.z - halfExtents.z && point.z <= center.z + halfExtents.z &&
            point.x >= center.x - halfExtents.x && point.x <= center.x + halfExtents.x) {
                        
            if (intersection < closestIntersection) {
                closestIntersection = intersection;
            }

            result = true;
        }
    }

 finished:
    if (t != NULL) {
        *t = closestIntersection;
    }

    return result;
}

inline bool TestPointAABB(vec3 p, vec3 min, vec3 max) {
    return (p.x > min.x && p.x < max.x) &&
        (p.y > min.y && p.y < max.y) &&
        (p.z > min.z && p.z < max.z);
}

// @NOTE: if the ray lies within the AABB then tMin will be 0.0f, and tMax is really the intersection we want
//        But its wasteful to test for the point inside the AABB so just use RaycastCube if thats what you want
inline bool RaycastAABB(vec3 aabbMin, vec3 aabbMax, Ray ray, real32 *tMin, bool testInside = false, real32 epsilon = FLT_EPSILON) {
    *tMin = 0.0f;
    r32 tMax = INFINITY;


    for (int i = 0; i < 3; i++) {
        if (Abs(ray.direction.data[i]) < epsilon) {
            if (ray.origin.data[i] < aabbMin.data[i] || ray.origin.data[i] > aabbMax.data[i]) { return false; }
        }
        else {
            r32 ood = 1.0f / ray.direction.data[i];
            r32 t1 = (aabbMin.data[i] - ray.origin.data[i]) * ood;
            r32 t2 = (aabbMax.data[i] - ray.origin.data[i]) * ood;

            if (t1 > t2) {
                r32 temp = t1; t1 = t2; t2 = temp;
            }

            *tMin = Max(*tMin, t1);
            tMax = Min(tMax, t2);

            if (*tMin > tMax) { return false; }
        }
    }
    
    if (testInside && TestPointAABB(ray.origin, aabbMin, aabbMax)) {
        *tMin = tMax;    
    }

    return true;
}

inline bool SegmentAABBIntersection(vec3 aabbMin, vec3 aabbMax, vec3 p0, vec3 p1, vec3 *point, real32 epsilon = FLT_EPSILON) {
    r32 tMin = 0.0f;
    r32 tMax = 1.0f;

    vec3 dir = p1 - p0;

    for (int i = 0; i < 3; i++) {
        if (Abs(dir.data[i]) < epsilon) {
            if (p0.data[i] < aabbMin.data[i] || p0.data[i] > aabbMax.data[i]) { return false; }
        }
        else {
            r32 ood = 1.0f / dir.data[i];
            r32 t1 = (aabbMin.data[i] - p0.data[i]) * ood;
            r32 t2 = (aabbMax.data[i] - p0.data[i]) * ood;

            if (t1 > t2) {
                r32 temp = t1; t1 = t2; t2 = temp;
            }

            tMin = Max(tMin, t1);
            tMax = Min(tMax, t2);

            if (tMin > tMax) { return false; }
        }
    }

    *point = Lerp(p0, p1, tMin);

    return true;
}



// Delaunay Triangulation
// [0] https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm
// [1] http://paulbourke.net/papers/triangulate/triangulate.c
struct Circle {
    vec2 center;
    real32 radius;
};

inline bool PointInCircle(Circle circle, vec2 point) {
    bool result = Distance(circle.center, point) <= circle.radius;
    return result;
}

inline bool PointInCircle(vec2 point, vec2 center, real32 radius) {
    bool result = Distance(center, point) <= radius;
    return result;
}

inline Circle Circumcircle(vec2 a, vec2 b, vec2 c) {
    Circle result = {};

    real32 m1, m2, max, mbx, may, mby;
    real32 dx, dy, drsqr;
    real32 fabsayby = fabsf(a.y - b.y);
    real32 fabsbycy = fabsf(b.y - c.y);

    /* Check for coincident points */
    if (fabsayby < FLT_EPSILON && fabsbycy < FLT_EPSILON) {
        return result;
    }

    if (fabsayby < FLT_EPSILON) {
        m2 = - (c.x-b.x) / (c.y-b.y);
        mbx = (b.x + c.x) / 2.0;
        mby = (b.y + c.y) / 2.0;
        result.center.x = (b.x + a.x) / 2.0;
        result.center.y = m2 * (result.center.x - mbx) + mby;
    }
    else if (fabsbycy < FLT_EPSILON) {
        m1 = - (b.x-a.x) / (b.y-a.y);
        max = (a.x + b.x) / 2.0;
        may = (a.y + b.y) / 2.0;
        result.center.x = (c.x + b.x) / 2.0;
        result.center.y = m1 * (result.center.x - max) + may;
    }
    else {
        m1 = - (b.x-a.x) / (b.y-a.y);
        m2 = - (c.x-b.x) / (c.y-b.y);
        max = (a.x + b.x) / 2.0;
        mbx = (b.x + c.x) / 2.0;
        may = (a.y + b.y) / 2.0;
        mby = (b.y + c.y) / 2.0;
        result.center.x = (m1 * max - m2 * mbx + mby - may) / (m1 - m2);
        if (fabsayby > fabsbycy) {
            result.center.y = m1 * (result.center.x - max) + may;
        } else {
            result.center.y = m2 * (result.center.x - mbx) + mby;
        }
    }

   dx = b.x - result.center.x;
   dy = b.y - result.center.y;
   result.radius = sqrtf(dx * dx + dy * dy);

    return result;
}

bool SegmentCircleIntersection(vec2 p0, vec2 p1, vec2 center, r32 radius, r32 *t) {
    vec2 seg = p1 - p0; // d 
    vec2 diff = p0 - center; // f

    r32 a = Dot(seg, seg);
    r32 b = 2 * Dot(diff, seg);
    r32 c = Dot(diff, diff) - (radius * radius);

    r32 discrim = b * b - 4 * a * c;
    if (discrim < 0) {
        return false;
    }
    else {
        discrim = sqrt(discrim);

        r32 t0 = (-b - discrim) / (2 * a);
        r32 t1 = (-b + discrim) / (2 * a);

        if (t0 >= 0 && t0 <= 1) {
            *t = t0;
            return true;
        }

        if (t1 >= 0 && t1 <= 1) {
            *t = t1;
            return true;
        }
    }

    return false;
}

struct DelaunayTriangleEdge {
    int32 indices[2];
    bool duplicated;

    int32& operator [](const int index) {
        return indices[index];
    }
};

inline bool TriangleEdgeEquals(DelaunayTriangleEdge a, DelaunayTriangleEdge b) {
    bool result = ((a[0] == b[0] && a[1] == b[1]) ||
                  (a[0] == b[1] && a[1] == b[0]));

    return result;
}


// Convex Hull
inline int32 ConvexHullCompare(const void *aPtr, const void *bPtr) {
    vec2 a = *(vec2 *)aPtr;
    vec2 b = *(vec2 *)bPtr;

    int32 result;

    if (a.x == b.x) {
        if (a.y == b.y) {
            result = 0;
        }
        else if (a.y < b.y) {
            result = -1;
        }
        else {
            result = 1;
        }
    }
    else {
        if (a.x < b.x) {
            result = -1;
        }
        else {
            result = 1;
        }
    }

    return result;
}

// Input must be pre-sorted using ConvexHullCompare()
// [0] https://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain
inline uint32 ConvexHull(vec2 *points, uint32 pointCount, uint32 *hullIndices) {
    // Returns points in counter-clockwise order
    uint32 hullIndicesCount = 0;

    for (int i = 0; i < pointCount; i++) {
        while (hullIndicesCount >= 2 &&
               Cross(points[hullIndices[hullIndicesCount - 1]] - points[hullIndices[hullIndicesCount - 2]],
                     points[i] - points[hullIndices[hullIndicesCount - 2]]) <= 0) {
            hullIndicesCount--;
        }

        hullIndices[hullIndicesCount++] = i;
    }

    int t = hullIndicesCount + 1;
    for (int i = pointCount - 2; i >= 0; i--) {
        while (hullIndicesCount >= t &&
               Cross(points[hullIndices[hullIndicesCount - 1]] - points[hullIndices[hullIndicesCount - 2]],
                     points[i] - points[hullIndices[hullIndicesCount - 2]]) <= 0) {
            hullIndicesCount--;
        }

        // Don't add the last point because it was already added in the first loop
        if (i != 0) {
            hullIndices[hullIndicesCount++] = i;
        }
    }

    return hullIndicesCount;
};

inline bool PointInConvexHull(vec2 *points, uint32 *hullIndices, uint32 hullIndicesCount, vec2 point) {
    vec2 scanline = V2(1, 0);
    vec2 scanlinePerp = V2(0, 1);

    int intersections = 0;

    for (int i = 0; i < hullIndicesCount; i++) {
        vec2 a = points[hullIndices[i]];
        vec2 b = points[hullIndices[(i + 1) % hullIndicesCount]];

        vec2 u = b - a;
        if (NearlyEquals(Dot(u, scanlinePerp), 0.0f)) {
            continue;
        }

        vec2 uPerp = V2(-u.y, u.x);
        vec2 w = a - point;

        real32 s = -Dot(scanlinePerp, w) / Dot(scanlinePerp, u);
        real32 t = Dot(uPerp, w) / Dot(uPerp, scanline);

        if (0 <= s && s <= 1 && t >= 0) {
            intersections++;
        }
    }

    bool pointInHull = (intersections % 2) == 1;
    return pointInHull;
}

inline void Quickhull(vec3 *points, u32 pointCount, vec3 *hullPoints, u32 hullPointCount) {
    // initial hull is most extreme points along axes, 6 points total
    // find the pair furthest apart
    // find the furthest point from this line
    // we have an initial hull
}


// GJK
// [0] http://www.cs.sjsu.edu/faculty/pollett/masters/Semesters/Spring12/josh/GJK.html
// [1] http://mollyrocket.com/849

typedef vec2 Support2DFunc(void *shape, vec2 direction);
typedef vec3 Support3DFunc(void *shape, vec3 direction);

inline vec3 ComputeSupportPoint(void *aShape, vec3 posA, quaternion rotA, vec3 scaleA, Support3DFunc *aSupport,
                                void *bShape, vec3 posB, quaternion rotB, vec3 scaleB, Support3DFunc *bSupport,
                                vec3 direction, vec3 *aSupportPoint, vec3 *bSupportPoint) {

    vec3 aDirection = DirectionToLocalSpace(rotA, direction);
    vec3 bDirection = DirectionToLocalSpace(rotB, direction);

    *aSupportPoint = MultiplyPoint(TRS(posA, rotA, scaleA), aSupport(aShape, aDirection));
    *bSupportPoint = MultiplyPoint(TRS(posB, rotB, scaleB), bSupport(bShape, -bDirection));

    vec3 supportPoint = *aSupportPoint - *bSupportPoint;

    return supportPoint;
}

struct Simplex3D {
    vec3 closestPoint; // @NOTE: on the simplex
    uint32 pointCount;
    vec3 points[4];

    vec3 closestPointA;
    vec3 closestPointB;

    // @NOTE: these are in world space of the shapes
    vec3 supportA[4];
    vec3 supportB[4];
};

inline bool SameDirection(vec3 A, vec3 B) {
    return Dot(A, B) > 0;
}

inline bool EvolveSimplex3D(Simplex3D *simplex, vec3 *direction) {
    bool collisionDetected = false;

    if (simplex->pointCount == 2) {
        vec3 AO = -simplex->points[1];
        vec3 AB = simplex->points[0] - simplex->points[1];

        if (SameDirection(AB, AO)) {
            *direction = Cross(Cross(AB, AO), AB);
        }
        else {
            // Remove B
            simplex->points[0] = simplex->points[1];
            simplex->supportA[0] = simplex->supportA[1];
            simplex->supportB[0] = simplex->supportB[1];
            simplex->pointCount--;

            *direction = AO;
        }

    }
    else if (simplex->pointCount == 3) {
        vec3 AO = -simplex->points[2];
        vec3 AB = simplex->points[1] - simplex->points[2];
        vec3 AC = simplex->points[0] - simplex->points[2];
        vec3 ABC = Cross(AB, AC);

        if (SameDirection(Cross(ABC, AC), AO)) {
            if (Dot(AC, AO) > 0.0f) {
                // Remove B
                simplex->points[1] = simplex->points[2];

                simplex->supportA[1] = simplex->supportA[2];
                simplex->supportB[1] = simplex->supportB[2];
                
                simplex->pointCount--;

                *direction = Cross(Cross(AC, AO), AC);
            }
            else {
                if (SameDirection(AB, AO)) {
                    // Remove C
                    simplex->points[0] = simplex->points[1];
                    simplex->points[1] = simplex->points[2];

                    simplex->supportA[0] = simplex->supportA[1];
                    simplex->supportA[1] = simplex->supportA[2];
                    
                    simplex->supportB[0] = simplex->supportB[1];
                    simplex->supportB[1] = simplex->supportB[2];
                    
                    simplex->pointCount--;

                    *direction = Cross(Cross(AB, AO), AB);
                }
                else {
                    // Remove C and B
                    simplex->points[0] = simplex->points[2];

                    simplex->supportA[0] = simplex->supportA[2];
                    simplex->supportB[0] = simplex->supportB[2];
                    
                    simplex->pointCount -= 2;

                    *direction = AO;
                }
            }
        }
        else {
            if (SameDirection(Cross(AB, ABC), AO)) {
                if (Dot(AB, AO) > 0.0f) {
                    // Remove C
                    simplex->points[0] = simplex->points[1];
                    simplex->points[1] = simplex->points[2];

                    simplex->supportA[0] = simplex->supportA[1];
                    simplex->supportA[1] = simplex->supportA[2];
                    
                    simplex->supportB[0] = simplex->supportB[1];
                    simplex->supportB[1] = simplex->supportB[2];
                    
                    simplex->pointCount--;

                    *direction = Cross(Cross(AB, AO), AB);
                }
                else {
                    // Remove C and B
                    simplex->points[0] = simplex->points[2];

                    simplex->supportA[0] = simplex->supportA[2];
                    simplex->supportB[0] = simplex->supportB[2];
                    
                    simplex->pointCount -= 2;

                    *direction = AO;
                }
            }
            else {
                if (SameDirection(ABC, AO)) {
                    *direction = ABC;

                }
                else {
                    // Swap B and C
                    vec3 C = simplex->points[0];
                    simplex->points[0] = simplex->points[1];
                    simplex->points[1] = C;

                    vec3 supportA = simplex->supportA[0];
                    simplex->supportA[0] = simplex->supportA[1];
                    simplex->supportA[1] = supportA;

                    vec3 supportB = simplex->supportB[0];
                    simplex->supportB[0] = simplex->supportB[1];
                    simplex->supportB[1] = supportB;

                    *direction = -ABC;
                }
            }
        }
    }
    else if (simplex->pointCount == 4) {
        vec3 AO = -simplex->points[3];
        vec3 AB = simplex->points[2] - simplex->points[3];
        vec3 AC = simplex->points[1] - simplex->points[3];
        vec3 AD = simplex->points[0] - simplex->points[3];

        vec3 ABC = Cross(AB, AC);
        vec3 ACD = Cross(AC, AD);
        vec3 ADB = Cross(AD, AB);

        // the side (positive or negative) of B, C and D
        // relative to the planes of ACD, ADB and ABC
        // respectively
        int32 BsideOnACD = Signum(Dot(ACD, AB));
        int32 CsideOnADB = Signum(Dot(ADB, AC));
        int32 DsideOnABC = Signum(Dot(ABC, AD));

        // whether the origin is on the same side of
        // ACD/ADB/ABC as B, C and D respectively
        bool ABsameAsOrigin = Signum(Dot(ACD, AO)) == BsideOnACD;
        bool ACsameAsOrigin = Signum(Dot(ADB, AO)) == CsideOnADB;
        bool ADsameAsOrigin = Signum(Dot(ABC, AO)) == DsideOnABC;

        // if the origin is on the same side as all B, C
        // and D, the origin is inside the tetrahedron and
        // thus there is a collision
        if (ABsameAsOrigin && ACsameAsOrigin && ADsameAsOrigin) {
            collisionDetected = true;
        }
        // if the origin is not on the side of B relative to ACD
        else if (!ABsameAsOrigin) {
            // B is farthest from the origin among all of
            // the tetrahedron’s points, so remove it from
            // the list and go on with the triangle case
            simplex->points[2] = simplex->points[3];

            simplex->supportA[2] = simplex->supportA[3];
            simplex->supportB[2] = simplex->supportB[3];
            
            simplex->pointCount -= 1;

            // the new direction is on the other side of
            // ACD, relative to B
            *direction = ACD * -BsideOnACD;
        }
        // if the origin is not on the side of C relative
        // to ADB
        else if (!ACsameAsOrigin) {
            // C is farthest from the origin among all of
            // the tetrahedron’s points, so remove it from
            // the list and go on with the triangle case
            simplex->points[1] = simplex->points[2];
            simplex->points[2] = simplex->points[3];

            simplex->supportA[1] = simplex->supportA[2];
            simplex->supportA[2] = simplex->supportA[3];

            simplex->supportB[1] = simplex->supportB[2];
            simplex->supportB[2] = simplex->supportB[3];
            
            simplex->pointCount -= 1;

            // the new direction is on the other side of
            // ADB, relative to C
            *direction = ADB * -CsideOnADB;
        }
        // if the origin is not on the side of D relative to ABC
        else if (!ADsameAsOrigin) {
            // D is farthest from the origin among all of
            // the tetrahedron’s points, so remove it from
            simplex->points[0] = simplex->points[1];
            simplex->points[1] = simplex->points[2];
            simplex->points[2] = simplex->points[3];

            simplex->supportA[0] = simplex->supportA[1];
            simplex->supportA[1] = simplex->supportA[2];
            simplex->supportA[2] = simplex->supportA[3];

            simplex->supportB[0] = simplex->supportB[1];
            simplex->supportB[1] = simplex->supportB[2];
            simplex->supportB[2] = simplex->supportB[3];
            
            simplex->pointCount -= 1;

            // the new direction is on the other side of
            // ABC, relative to D
            *direction = ABC * -DsideOnABC;
        }

        // Go on with the triangle case
        if (!collisionDetected) {
            collisionDetected =  EvolveSimplex3D(simplex, direction);
        }
    }

    return collisionDetected;
}

inline bool GJK3D(void *aShape, vec3 posA, quaternion rotA, vec3 scaleA, Support3DFunc *aSupport,
                 void *bShape, vec3 posB, quaternion rotB, vec3 scaleB, Support3DFunc *bSupport,
                 Simplex3D *finalSimplex, real32 *distance) {
    
    vec3 direction = V3(1, 0, 0);
    vec3 aSupportPoint = {};
    vec3 bSupportPoint = {};

    Simplex3D simplex = {};

    
    vec3 nextPoint = ComputeSupportPoint(aShape, posA, rotA, scaleA, aSupport,
                                         bShape, posB, rotB, scaleA, bSupport,
                                         direction, &aSupportPoint, &bSupportPoint);

    simplex.points[simplex.pointCount] = nextPoint;
    simplex.closestPoint = nextPoint;
    simplex.supportA[simplex.pointCount] = aSupportPoint;
    simplex.supportB[simplex.pointCount] = bSupportPoint;
    simplex.pointCount++;
    
    direction = -nextPoint;

    // @NOTE: this is so we can avoid an infinity loop where the direction never changes
    // and our dot <= epsilon is never passing
    vec3 prevDirection = {};
    while (true) {
        //ASSERT(!(direction.x == 0 && direction.y == 0 && direction.z == 0));
        
        nextPoint = ComputeSupportPoint(aShape, posA, rotA, scaleA, aSupport,
                                        bShape, posB, rotB, scaleB, bSupport,
                                        direction, &aSupportPoint, &bSupportPoint);

        //http://www.dyn4j.org/2010/04/gjk-distance-closest-points/#gjk-minkowski
        real32 epsilon = 1.0e-4;
        real32 dot = Dot(simplex.points[0], direction);
        //if (Dot(nextPoint, direction) < 0.0f) {

        // @NOTE: instead of just checking if nextPoint and direction is extremal, we check if the difference
        // between it and a point in the simplex is less than some threshold
        if (Dot(nextPoint, direction) - dot <= epsilon || prevDirection == direction) {

            if (simplex.pointCount == 1) {
                simplex.closestPoint = simplex.points[0];
                simplex.closestPointA = simplex.supportA[0];
                simplex.closestPointB = simplex.supportB[0];
            }

            if (simplex.pointCount == 2) {
                real32 t;
                simplex.closestPoint = ClosestPointOnSegment(V3(0), simplex.points[0], simplex.points[1], &t);
                simplex.closestPointA = Lerp(simplex.supportA[0], simplex.supportA[1], t);
                simplex.closestPointB = Lerp(simplex.supportB[0], simplex.supportB[1], t);
            }

            if (simplex.pointCount == 3) {
                simplex.closestPoint = ClosestPointOnTriangle(simplex.points[0], simplex.points[1],
                                                              simplex.points[2], V3(0));
                vec3 barycentric = CartesianToBarycentric(simplex.points[0], simplex.points[1], simplex.points[2],
                                                          simplex.closestPoint);
                vec3 sumA = {};
                vec3 sumB = {};
                for (int i = 0; i < simplex.pointCount; i++) {
                    sumA = sumA + (simplex.supportA[i] * barycentric.data[i]);
                    sumB = sumB + (simplex.supportB[i] * barycentric.data[i]);
                }

                simplex.closestPointA = sumA;
                simplex.closestPointB = sumB;
            }

            *distance = Length(simplex.closestPoint);
            *finalSimplex = simplex;            
            return false;
        }

        simplex.supportA[simplex.pointCount] = aSupportPoint;
        simplex.supportB[simplex.pointCount] = bSupportPoint;
        simplex.points[simplex.pointCount++] = nextPoint;

        prevDirection = direction;
        if (EvolveSimplex3D(&simplex, &direction)) {
            simplex.closestPoint = ClosestPointOnTetrahedron(V3(0), simplex.points[3], simplex.points[2],
                                                              simplex.points[1], simplex.points[0]);
            *finalSimplex = simplex;
            return true;
        }
    }
}

vec3 CalculateNormal(vec3 *verts, u32 vertCount) {
    vec3 normal = {};

    // We were doing Newell's method, but some components werent zeroed out
    // like you'd expect them to be
    for (int i = 0; i < vertCount; i++) {
        vec3 curr = verts[i];
        vec3 next = verts[(i + 1) % vertCount];

        normal.x += (curr.y - next.y) * (curr.z + next.z);
        normal.y += (curr.z - next.z) * (curr.x + next.x);
        normal.z += (curr.x - next.x) * (curr.y + next.y);
    }

    return Normalize(normal);


    // @BUG: with this method we need to ensure that the two vectors aren't parallel!
    // Why not just calculate with 3 verts?
    //normal = Normalize(Cross(verts[1] - verts[0], verts[2] - verts[1]));
    //return normal;
}

// @NOTE: assumes consistent winding
void TriangulateConvexPolygon(u32 vertCount, u32 *indices, u32 *indexCount, u32 vertOffset = 0, bool reverseWinding = false) {
    ASSERT(vertCount > 2);
    u32 support = 1 + vertOffset;
    
    while (support + 1 < vertCount + vertOffset) {
        u32 temp = support + 1;

        if (reverseWinding) {
            indices[*indexCount + 2] = vertOffset;
            indices[*indexCount + 1] = support;
            indices[*indexCount + 0] = temp;
        }
        else {
            indices[*indexCount + 0] = vertOffset;
            indices[*indexCount + 1] = support;
            indices[*indexCount + 2] = temp;
        }

        *indexCount += 3;
        
        support = temp;
    }
}

bool VectorsAreParallel(vec3 a, vec3 b) {
    return Dot(a, a) * Dot(b, b) - Dot(a, b) * Dot(a, b) == 0;
}

bool PointInFOV(vec3 apex, vec3 dir, real32 fov, vec3 p) {
    vec3 diff = Normalize(p - dir);
    real32 dot = Dot(dir, diff);
    return Dot(dir, diff) > fov;
}

// Add to the bottom of the geometry.h file

struct Rect {
    vec2 min;
    vec2 max;
};

Rect MakeRect(vec2 position, vec2 halfSize) {
    Rect r;
    r.min = position - halfSize;
    r.max = position + halfSize;
    return r;
}

// @NOTE: inclusive
bool PointRectTest(Rect r, vec2 p) {
    if (p.x < r.min.x || p.x > r.max.x) {
        return false;
    }

    if (p.y < r.min.y || p.y > r.max.y) {
        return false;
    }

    return (p.x >= r.min.x && p.x <= r.max.x) && (p.y >= r.min.y && p.y <= r.max.y);
}

Rect GlobalRect(vec2 position, Rect rect) {
    Rect result;

    result.min = rect.min + position;
    result.max = rect.max + position;

    return result;
}

inline bool TestPointAABB(vec2 p, vec2 min, vec2 max) {
    return (p.x > min.x && p.x < max.x) &&
        (p.y > min.y && p.y < max.y);
}

inline bool RaycastAABB(vec2 aabbMin, vec2 aabbMax, Ray ray, real32 *tMin, bool testInside = false, real32 epsilon = FLT_EPSILON) {
    *tMin = 0.0f;
    r32 tMax = INFINITY;

    for (int i = 0; i < 2; i++) {
        if (Abs(ray.direction.data[i]) < epsilon) {
            if (ray.origin.data[i] < aabbMin.data[i] || ray.origin.data[i] > aabbMax.data[i]) { return false; }
        }
        else {
            r32 ood = 1.0f / ray.direction.data[i];
            r32 t1 = (aabbMin.data[i] - ray.origin.data[i]) * ood;
            r32 t2 = (aabbMax.data[i] - ray.origin.data[i]) * ood;

            if (t1 > t2) {
                r32 temp = t1; t1 = t2; t2 = temp;
            }

            *tMin = Max(*tMin, t1);
            tMax = Min(tMax, t2);

            if (*tMin > tMax) { return false; }
        }
    }
    
    if (testInside && TestPointAABB(V2(ray.origin.x, ray.origin.y), aabbMin, aabbMax)) {
        *tMin = tMax;    
    }

    return true;
}


// This function takes a pointer to a vec2. That means when we change the values of dir, we are
// changing the values at the memory address we passed in!
// We WANT the value of dir to change based on the computation of this function. IMPORTANT
// This uses the SeparatingAxisTheorem
// @NOTE: this will not set the component of dir that doesn't need to be moved, so make sure it's cleared
bool RectTest(Rect a, Rect b, vec2 aPosition, vec2 bPosition, vec2 *dir) {

    Rect aGlobal;
    aGlobal.min = (a.min + aPosition);
    aGlobal.max = (a.max + aPosition);

    Rect bGlobal;
    bGlobal.min = (b.min + bPosition);
    bGlobal.max = (b.max + bPosition);

    // Is the bug that the positions can be negative?
    r32 lengthX = Min(aGlobal.max.x, bGlobal.max.x) - Max(aGlobal.min.x, bGlobal.min.x);
    r32 lengthY = Min(aGlobal.max.y, bGlobal.max.y) - Max(aGlobal.min.y, bGlobal.min.y);

    // This tells us if there is separation on either axis
    if (lengthX < 0) { return false; }
    if (lengthY < 0) { return false; }
    // If we get here there is no separation, and we want to find the axis with the least length

    r32 posDiffX = bPosition.x - aPosition.x;
    r32 posDiffY = bPosition.y - aPosition.y;

    if (lengthX < lengthY) {
        dir->x = lengthX * -Sign(posDiffX);
    }
    else {
        dir->y = lengthY * -Sign(posDiffY);
    }

    return true;
}


#endif

