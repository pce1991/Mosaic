
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

inline real32 PerpProduct(vec2 a, vec2 b) {
    return a.y * b.x - a.x * b.y;
}

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


struct Ray2D {
    vec2 origin;
    vec2 direction;
};

inline Ray2D MakeRay(vec2 origin, vec2 direction) {
    Ray2D result;
    result.origin = origin;
    result.direction = direction;

    return result;
}

inline vec2 PointAt(Ray2D ray, real32 t) {
    vec2 result = ray.origin + t * ray.direction;
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

inline bool RaycastAABB(vec2 aabbMin, vec2 aabbMax, vec2 origin, vec2 direction, real32 *tMin, bool testInside = false, real32 epsilon = FLT_EPSILON) {
    *tMin = 0.0f;
    r32 tMax = INFINITY;

    for (int i = 0; i < 2; i++) {
        if (Abs(direction.data[i]) < epsilon) {
            if (origin.data[i] < aabbMin.data[i] || origin.data[i] > aabbMax.data[i]) { return false; }
        }
        else {
            r32 ood = 1.0f / direction.data[i];
            r32 t1 = (aabbMin.data[i] - origin.data[i]) * ood;
            r32 t2 = (aabbMax.data[i] - origin.data[i]) * ood;

            if (t1 > t2) {
                r32 temp = t1; t1 = t2; t2 = temp;
            }

            *tMin = Max(*tMin, t1);
            tMax = Min(tMax, t2);

            if (*tMin > tMax) { return false; }
        }
    }
    
    if (testInside && TestPointAABB(V2(origin.x, origin.y), aabbMin, aabbMax)) {
        *tMin = tMax;    
    }

    return true;
}

inline bool RaycastAABB(vec2 aabbMin, vec2 aabbMax, Ray2D ray, real32 *tMin, bool testInside = false, real32 epsilon = FLT_EPSILON) {
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


// This function takes a pointer to a vec2. That means when we change the values of dir,
// we are changing the values at the memory address we passed in! We WANT the value of dir
// to change based on the computation of this function.
// This uses the SeparatingAxisTheorem
// @NOTE: this will not set the component of dir that doesn't need
// to be moved, so make sure it's cleared Dir will push A out of B
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
    
    if (lengthX < lengthY) {
        if (bGlobal.max.x < aGlobal.max.x) {
            dir->x = Abs(bGlobal.max.x - aGlobal.min.x);
        }
        else {
            dir->x = -Abs(bGlobal.min.x - aGlobal.max.x);
        }
    }
    else {
        if (bGlobal.max.y < aGlobal.max.y) {
            dir->y = Abs(bGlobal.max.y - aGlobal.min.y);
        }
        else {
            dir->y = -Abs(bGlobal.min.y - aGlobal.max.y);
        }
    }


    return true;
}

