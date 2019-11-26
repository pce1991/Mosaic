

inline real32 BezierQuadratic(real32 a, real32 b, real32 c, real32 t) {
    real32 t2 = t * t;
    real32 mt = 1 - t;
    real32 mt2 = mt * mt;

    return (mt2 * a) + (2 * mt * t * b) + (t2 * c);
}

inline real32 BezierCubic(real32 a, real32 b, real32 c, real32 d, real32 t) {
    real32 t2 = t * t;
    real32 t3 = t * t * t;
    real32 mt = 1 - t;
    real32 mt2 = mt * mt;
    real32 mt3 = mt * mt * mt;

    return (mt3 * a) + (3 * mt2 * t * b) + (3 * mt * t2 * c) + (t3 * d);
}

inline real32 BezierCubicDerivative(real32 a, real32 b, real32 c, real32 d, real32 t) {
    return BezierQuadratic(3 * (b - a), 3 * (c - b), 3 * (d - c), t);
}

inline real32 BezierCubicSecondDerivative(real32 a, real32 b, real32 c, real32 d, real32 t) {
    return 6 * ((1 - t) * (c - 2 * b + a) + t * (d - 2 * c + b));
}

inline vec2 BezierQuadratic(vec2 a, vec2 b, vec2 c, real32 t) {
    return V2(BezierQuadratic(a.x, b.x, c.x, t),
              BezierQuadratic(a.y, b.y, c.y, t));
}

inline vec2 BezierCubic(vec2 a, vec2 b, vec2 c, vec2 d, real32 t) {
    return V2(BezierCubic(a.x, b.x, c.x, d.x, t),
              BezierCubic(a.y, b.y, c.y, d.y, t));
}

inline vec2 BezierCubicDerivative(vec2 a, vec2 b, vec2 c, vec2 d, real32 t) {
    return V2(BezierCubicDerivative(a.x, b.x, c.x, d.x, t),
              BezierCubicDerivative(a.y, b.y, c.y, d.y, t));
}

inline vec3 BezierQuadratic(vec3 a, vec3 b, vec3 c, real32 t) {
    return V3(BezierQuadratic(a.x, b.x, c.x, t),
              BezierQuadratic(a.y, b.y, c.y, t),
              BezierQuadratic(a.z, b.z, c.z, t));
}

inline vec3 BezierCubic(vec3 a, vec3 b, vec3 c, vec3 d, real32 t) {
    return V3(BezierCubic(a.x, b.x, c.x, d.x, t),
              BezierCubic(a.y, b.y, c.y, d.y, t),
              BezierCubic(a.z, b.z, c.z, d.z, t));
}

inline vec3 BezierCubicDerivative(vec3 a, vec3 b, vec3 c, vec3 d, real32 t) {
    return V3(BezierCubicDerivative(a.x, b.x, c.x, d.x, t),
              BezierCubicDerivative(a.y, b.y, c.y, d.y, t),
              BezierCubicDerivative(a.z, b.z, c.z, d.z, t));
}

inline vec3 BezierCubicSecondDerivative(vec3 a, vec3 b, vec3 c, vec3 d, real32 t) {
    return V3(BezierCubicSecondDerivative(a.x, b.x, c.x, d.x, t),
              BezierCubicSecondDerivative(a.y, b.y, c.y, d.y, t),
              BezierCubicSecondDerivative(a.z, b.z, c.z, d.z, t));
}


static real32 gaussQuadratureWeights64[] = {
    0.0486909570091397f, 0.0486909570091397f, 0.0485754674415034f, 0.0485754674415034f,
    0.0483447622348030f, 0.0483447622348030f, 0.0479993885964583f, 0.0479993885964583f,
    0.0475401657148303f, 0.0475401657148303f, 0.0469681828162100f, 0.0469681828162100f,
    0.0462847965813144f, 0.0462847965813144f, 0.0454916279274181f, 0.0454916279274181f,
    0.0445905581637566f, 0.0445905581637566f, 0.0435837245293235f, 0.0435837245293235f,
    0.0424735151236536f, 0.0424735151236536f, 0.0412625632426235f, 0.0412625632426235f,
    0.0399537411327203f, 0.0399537411327203f, 0.0385501531786156f, 0.0385501531786156f,
    0.0370551285402400f, 0.0370551285402400f, 0.0354722132568824f, 0.0354722132568824f,
    0.0338051618371416f, 0.0338051618371416f, 0.0320579283548516f, 0.0320579283548516f,
    0.0302346570724025f, 0.0302346570724025f, 0.0283396726142595f, 0.0283396726142595f,
    0.0263774697150547f, 0.0263774697150547f, 0.0243527025687109f, 0.0243527025687109f,
    0.0222701738083833f, 0.0222701738083833f, 0.0201348231535302f, 0.0201348231535302f,
    0.0179517157756973f, 0.0179517157756973f, 0.0157260304760247f, 0.0157260304760247f,
    0.0134630478967186f, 0.0134630478967186f, 0.0111681394601311f, 0.0111681394601311f,
    0.0088467598263639f, 0.0088467598263639f, 0.0065044579689784f, 0.0065044579689784f,
    0.0041470332605625f, 0.0041470332605625f, 0.0017832807216964f, 0.0017832807216964f,
};

static real32 gaussQuadratureAbscissae64[] = {
    -0.0243502926634244f, 0.0243502926634244f, -0.0729931217877990f, 0.0729931217877990f,
    -0.1214628192961206f, 0.1214628192961206f, -0.1696444204239928f, 0.1696444204239928f,
    -0.2174236437400071f, 0.2174236437400071f, -0.2646871622087674f, 0.2646871622087674f,
    -0.3113228719902110f, 0.3113228719902110f, -0.3572201583376681f, 0.3572201583376681f,
    -0.4022701579639916f, 0.4022701579639916f, -0.4463660172534641f, 0.4463660172534641f,
    -0.4894031457070530f, 0.4894031457070530f, -0.5312794640198946f, 0.5312794640198946f,
    -0.5718956462026340f, 0.5718956462026340f, -0.6111553551723933f, 0.6111553551723933f,
    -0.6489654712546573f, 0.6489654712546573f, -0.6852363130542333f, 0.6852363130542333f,
    -0.7198818501716109f, 0.7198818501716109f, -0.7528199072605319f, 0.7528199072605319f,
    -0.7839723589433414f, 0.7839723589433414f, -0.8132653151227975f, 0.8132653151227975f,
    -0.8406292962525803f, 0.8406292962525803f, -0.8659993981540928f, 0.8659993981540928f,
    -0.8893154459951141f, 0.8893154459951141f, -0.9105221370785028f, 0.9105221370785028f,
    -0.9295691721319396f, 0.9295691721319396f, -0.9464113748584028f, 0.9464113748584028f,
    -0.9610087996520538f, 0.9610087996520538f, -0.9733268277899110f, 0.9733268277899110f,
    -0.9833362538846260f, 0.9833362538846260f, -0.9910133714767443f, 0.9910133714767443f,
    -0.9963401167719553f, 0.9963401167719553f, -0.9993050417357722f, 0.9993050417357722f,
};

inline real32 ArcLength(vec3 a, vec3 b, vec3 c, vec3 d, real32 t) {
    static uint32 tableSize = 64;
    static real32 *weights = gaussQuadratureWeights64;
    static real32 *abscissae = gaussQuadratureWeights64;

    real32 result = 0.0f;

    for (int i = 0; i < tableSize; i++) {
        real32 iterationT = 0.5f * t * (abscissae[i] + 1.0f);
        real32 lgq = Length(BezierCubicDerivative(a, b, c, d, iterationT));

        result = result + weights[i] * lgq;
    }

    result = t * 0.5f * result;

    return result;
}


// Reparameterization
// [0] https://www.geometrictools.com/Documentation/MovingAlongCurveSpecifiedSpeed.pdf
inline void ReparameterizeByArcLength(vec3 a, vec3 b, vec3 c, vec3 d, uint32 sampleCount, real32 *tSamples) {

    static uint32 iterations = 32;

    ASSERT(sampleCount >= 2);
    real32 arcLength = ArcLength(a, b, c, d, 1.0f);

    for (int i = 0; i < sampleCount; i++) {
        real32 s = i / (real32)(sampleCount - 1);

        real32 tMin = 0.0f;
        real32 tMax = 1.0f;

        // Initial guess for Newton's method
        real32 t = s;

        // Hybrid Newton-Raphson with bisection
        real32 lower = tMin;
        real32 upper = tMax;

        for (int iteration = 0; iteration < iterations; iteration++) {
            real32 F = ArcLength(a, b, c, d, t) / arcLength - s;
            if (NearlyEquals(F, 0.0f, FLT_EPSILON, 0.0f)) {
                break;
            }

            real32 DF = Length(BezierCubicDerivative(a, b, c, d, t));
            real32 tCandidate = t - F / DF;

            if (F > 0) {
                upper = t;
                if (tCandidate <= lower) {
                    t = 0.5f * (upper + lower);
                }
                else {
                    t = tCandidate;
                }
            }
            else {
                lower = t;
                if (tCandidate >= upper) {
                    t = 0.5f * (upper + lower);
                }
                else {
                    t = tCandidate;
                }
            }
        }

        tSamples[i] = t;
    }

    tSamples[0] = 0.0f;
    tSamples[sampleCount - 1] = 1.0f;
}

inline real32 ReparameterizedByArcLengthEvaluate(uint32 sampleCount, real32 *tSamples, real32 s) {
    ASSERT(s >= 0.0f);
    ASSERT(s <= 1.0f);

    int32 index0 = (int32)floorf(s * (sampleCount - 1));
    int32 index1 = (int32)ceilf(s * (sampleCount - 1));

    real32 blend = s * (sampleCount - 1) - index0;
    real32 result = Lerp(tSamples[index0], tSamples[index1], blend);

    return result;
}


// TODO: use reparameterization to get equal length segments
inline void ApproximateCubicBezier(vec3 a, vec3 b, vec3 c, vec3 d, uint32 segments, vec3 *points) {
    real32 t = 0.0f;
    vec3 currPoint;
    vec3 prevPoint = a;

    points[0] = a;

    // <= because points has segments + 1 members
    for (int i = 1; i <= segments; i++) {
        t = i / (real32)segments;
        vec3 val = BezierCubic(a, b, c, d, t);

        currPoint = val;
        points[i] = val;
        prevPoint = currPoint;
    }
}

inline real32 ApproximateBezierLength(vec3 a, vec3 b, vec3 c, vec3 d, uint32 segments, vec3 *points) {
    ApproximateCubicBezier(a, b, c, d, segments, points);
    real32 length = 0.0f;
    for (int i = 0; i < segments; i++) {
        length += Distance(points[i], points[i + 1]);
    }

    return length;
}


inline void BezierCubicSplit(real32 a, real32 b, real32 c, real32 d, real32 t, real32 *a0, real32 *b0, real32 *c0, real32 *d0, real32 *a1, real32 *b1, real32 *c1, real32 *d1) {

    real32 ab = Lerp(a, b, t);
    real32 bc = Lerp(b, c, t);
    real32 cd = Lerp(c, d, t);

    real32 abc = Lerp(ab, bc, t);
    real32 bcd = Lerp(bc, cd, t);

    real32 p = BezierCubic(a, b, c, d, t);

    *a0 = a;
    *b0 = ab;
    *c0 = abc;
    *d0 = p;

    *a1 = p;
    *b1 = bcd;
    *c1 = cd;
    *d1 = d;
}

inline void BezierCubicSplit(vec2 a, vec2 b, vec2 c, vec2 d, real32 t, vec2 *a0, vec2 *b0, vec2 *c0, vec2 *d0, vec2 *a1, vec2 *b1, vec2 *c1, vec2 *d1) {
    BezierCubicSplit(a.x, b.x, c.x, d.x, t, &a0->x, &b0->x, &c0->x, &d0->x, &a1->x, &b1->x, &c1->x, &d1->x);
    BezierCubicSplit(a.y, b.y, c.y, d.y, t, &a0->y, &b0->y, &c0->y, &d0->y, &a1->y, &b1->y, &c1->y, &d1->y);
}

inline void BezierCubicSplit(vec3 a, vec3 b, vec3 c, vec3 d, real32 t, vec3 *a0, vec3 *b0, vec3 *c0, vec3 *d0, vec3 *a1, vec3 *b1, vec3 *c1, vec3 *d1) {
    BezierCubicSplit(a.x, b.x, c.x, d.x, t, &a0->x, &b0->x, &c0->x, &d0->x, &a1->x, &b1->x, &c1->x, &d1->x);
    BezierCubicSplit(a.y, b.y, c.y, d.y, t, &a0->y, &b0->y, &c0->y, &d0->y, &a1->y, &b1->y, &c1->y, &d1->y);
    BezierCubicSplit(a.z, b.z, c.z, d.z, t, &a0->z, &b0->z, &c0->z, &d0->z, &a1->z, &b1->z, &c1->z, &d1->z);
}

inline bool IsMonotonic(vec2 a, vec2 b) {
    bool result = false;
    real32 bDistance = sqrtf(Square(b.x - a.x));
    if (bDistance > 0) {
        result = true;
    }
    return result;
}

// http://edmund.birotanker.com/monotonic-bezier-curves-for-animation.html
real32 TValueFromX(vec2 a, vec2 b, vec2 c, vec2 d, real32 x) {
    real32 t = 0;

    real32 bDistance = Abs(a.x - b.x);
    real32 cDistance = Abs(c.x - d.x);

    ASSERT(b.x != 1.0f && c.x != 1.0f);

    real32 D = (3.0f * bDistance) + (3.0f * cDistance) - 2.0f;
    real32 N = (2.0f * bDistance) + cDistance - 1;

    real32 R = (Square(N) - (bDistance * D)) / Square(D);
    real32 Q = (((3.0f * bDistance * D * N) - (2.0f * Cube(N))) / Cube(D)) - (x / D);
    real32 discrim = Square(Q) - (4 * Cube(R));

    ASSERT(discrim != 0);

    if (discrim > 0) {
        real32 Z = (0.5f * sqrtf(discrim));
        real32 WCube = (Q * -0.5f) + Z;
        if (WCube == 0) {
            WCube = (Q * -0.5f) - Z;
        }
        
        real32 W = cbrtf(WCube);
        real32 U = W + (R / W);
        t = U + (N / D);
    }
    else {
        real32 theta = acosf(-Q/(2 * sqrtf(Cube(R))));
        real32 phi = (theta / 3.0f) + ((2.0f * _2PI) / 3.0f);
        real32 U = 2.0f * sqrtf(R) * cosf(phi);
        t = U + (N / D);
    }

    //WARNING: there are times where D or N could be negative when the other isnt,
    //and the division fields a negative number whose absolute value is greater than U
    //This may be floating point error where c or d are too close to 1?

    return t;
}


