# Mosaic Core API Reference

Quick reference for the engine's core APIs. All types use `real32`/`float32` (float), `int32` (int), `uint32` (unsigned int).

---

## Math

### Types

| Type | Description |
|------|-------------|
| `vec2`, `vec3`, `vec4` | Float vectors with `.x .y .z .w` / `.r .g .b .a` accessors |
| `vec2i`, `vec3i`, `vec4i` | Integer vectors |
| `mat3`, `mat4` | Column-major matrices. Access via `.columns[i]`, `.data[]`, or `.mRC` |
| `quaternion` | Rotation quaternion `{x, y, z, w}` |
| `Plane` | `{point, normal, d}` |
| `Ray` | `{origin, direction}` (vec3) |
| `Ray2D` | `{origin, direction}` (vec2) |
| `Rect` | `{min, max}` (vec2 AABB) |

### Constants

```cpp
_PI, _2PI, SQRT2
UP      // V3(0, 1, 0)
LEFT    // V3(1, 0, 0)
FORWARD // V3(0, 0, 1)
XZ(a)   // V2(a.x, a.z)
```

---

### vector.h

**Constructors** -- all named `V2`, `V3`, `V4` (and `V2i`, `V3i`, `V4i` for int vectors). Overloaded for mixed int/float args.

```cpp
vec2 a = V2(1.0f, 2.0f);
vec3 b = V3(1, 2, 3);
vec4 c = V4(a, 1.0f);        // vec2 + w
vec3 d = V3(V2(1, 2), 3);    // vec2 + z
vec2 e = V2(5.0f);           // splat: {5, 5}
```

**Operators** -- `+`, `-`, `*`, `/`, `==`, `!=` for all vector types. Scalar ops use `real32` (or `int32` for `i` types).

| Function | Description |
|----------|-------------|
| `Hadamard(a, b)` | Component-wise multiply |
| `Dot(a, b)` | Dot product |
| `Cross(a, b)` | Cross product (2D returns scalar, 3D returns vec3) |
| `Length(a)` | Vector length |
| `LengthSq(a)` | Length squared (avoids sqrt) |
| `Distance(a, b)` | Distance between two points |
| `DistanceSq(a, b)` | Distance squared |
| `Normalize(a)` | Unit vector (safe: returns 0 on zero-length) |
| `Lerp(a, b, t)` | Linear interpolation |
| `NLerp(a, b, t)` | Normalized lerp |
| `Angle(a, b)` | Angle between vectors (radians) |
| `Clamp(a, min, max)` | Per-component clamp |
| `Min(a, b)` / `Max(a, b)` | Per-component min/max |
| `Abs(v)` | Per-component absolute value |
| `Rotate(angle, v)` | Rotate vec2 by angle in radians |
| `LeftHandPerp(v)` / `RightHandPerp(v)` | 2D perpendicular vectors |
| `Project(a, p)` | Project p onto a |
| `ProjectPointOntoLine(pt, a, b)` | Closest point on line segment |
| `NearlyEquals(a, b, epsilon)` | Approximate equality |
| `Round(v)` / `Ceilv(v)` | Per-component round/ceil |
| `RandDir()` | Random unit vec2 |

```cpp
float len = Length(V3(1, 2, 3));
vec3 dir = Normalize(V3(0, 1, 0));
vec3 mid = Lerp(a, b, 0.5f);
```

---

### scalar.h

| Function | Description |
|----------|-------------|
| `Lerp(a, b, t)` | Linear interpolation |
| `InverseLerp(a, b, x)` | Returns t where Lerp(a,b,t) == x |
| `LinearRemap(a, b, x, a2, b2)` | Map x from [a,b] to [a2,b2] |
| `Clamp(x, min, max)` | Clamp to range |
| `Clamp01(x)` | Clamp to [0,1] |
| `Min(a, b)` / `Max(a, b)` | Min/max (overloaded for int/float) |
| `Abs(x)` | Absolute value |
| `SafeRatio(a, b, n)` | a/b or n if b==0 |
| `SafeInvert(n)` | 1/n or 0 if n==0 |
| `SmoothStep(t)` | Hermite smoothstep (3t^2 - 2t^3) |
| `SmootherStep(t)` | Quintic smoothstep (6t^5 - 15t^4 + 10t^3) |
| `DegToRad(deg)` / `RadToDeg(rad)` | Angle conversion |
| `NormalizeAngleRad(a)` | Wrap angle to [0, 2pi] |
| `DeltaAngleRad(from, to)` | Shortest signed angle difference |
| `PingPong(t, max)` | Ping-pong oscillation |
| `Modf(x, m)` | Modulo (always positive) |
| `NearlyEquals(a, b, tolerance)` | Approximate equality |
| `InRange(n, min, max)` | Bounds check |
| `Square(x)` / `Cube(x)` | Power functions |
| `Snap(value, snapInterval)` | Snap value to nearest interval |
| `Log(base, x)` | Logarithm |
| `Powi(base, exp)` | Integer power |
| `Signum(x)` | Returns -1, 0, or 1 |
| `Ceilf(x)` / `Floorf(x)` / `Roundf(x)` / `Fractf(x)` | Rounding functions |

```cpp
float t = Lerp(0.0f, 10.0f, 0.75f);
float rad = DegToRad(90.0f);
```

---

### color.h

| Macro | Description |
|-------|-------------|
| `RGB(r, g, b)` | Creates vec4 from [0,1] floats |
| `RGB8(r, g, b)` | Creates vec4 from [0,255] ints |
| `RGBHex(ffeeaa)` | From hex literal (no 0x prefix) |
| `RGBH(0xffeeaa)` | From hex value |

Named colors: `WHITE`, `BLACK`, `RED`, `GREEN`, `BLUE`, `MAGENTA`, `YELLOW`, `CYAN`
Pastel colors: `PASTEL_RED`, `PASTEL_GREEN`, `PASTEL_BLUE`, `PASTEL_ORANGE`, `PASTEL_YELLOW`, `PASTEL_PURPLE`

| Function | Description |
|----------|-------------|
| `RGBToHSV(rgb)` | Convert RGB vec3 to HSV vec3 |
| `HSVToRGB(hsv)` | Convert HSV vec3 (H: [0,360]) to RGB vec3 |

```cpp
vec4 col = RGBHex(3498db);
vec4 red = RGB(1.0f, 0.0f, 0.0f);
```

---

### quaternion.h

| Function | Description |
|----------|-------------|
| `Quaternion(x, y, z, w)` | Constructor |
| `IdentityQuaternion()` | No rotation {0,0,0,1} |
| `AxisAngle(axis, rad)` | Quaternion from axis + angle |
| `ToAxisAngle(q, &axis, &rad)` | Decompose to axis + angle |
| `FromEulerAngles(heading, attitude, bank)` | From euler angles (radians) |
| `FromEulerAngles(vec3)` | From euler vec3 |
| `ToEulerAngles(q)` | Returns euler vec3 |
| `QuaternionWithXYZ(X, Y, Z)` | From orthonormal basis vectors |
| `QuaternionWithXY(X, Y)` / `QuaternionWithYZ(Y, Z)` | From 2 basis vectors |
| `QuaternionWithX(X)` | From single axis |
| `Normalize(q)` | Unit quaternion |
| `Inverse(q)` | Inverse rotation |
| `Dot(a, b)` | Dot product |
| `Angle(a, b)` | Angle between quaternions |
| `Rotate(q, p)` | Rotate vec3 p by quaternion |
| `RelativeQuaternion(a, b)` | q such that b * q == a |
| `Lerp(a, b, t)` | Normalized lerp |
| `Slerp(a, b, t)` | Spherical lerp |
| `Length(q)` / `LengthSq(q)` | Magnitude |
| `NearlyEquals(a, b, eps)` | Approximate equality |
| `HasNaN(q)` | Check for NaN components |
| `operator*` | Quaternion multiplication |

```cpp
quaternion rot = AxisAngle(UP, DegToRad(45.0f));
vec3 rotated = Rotate(rot, V3(1, 0, 0));
```

---

### matrix.h

| Function | Description |
|----------|-------------|
| `Zero3()` / `Zero4()` | Zero matrix |
| `Identity3()` / `Identity4()` | Identity matrix |
| `Translation4(vec3)` | Translation matrix |
| `Rotation4(quaternion)` | Rotation from quaternion |
| `Scale4(float)` / `Scale4(vec3)` | Uniform/non-uniform scale |
| `Translation3`, `Rotation3`, `Scale3` | 3x3 versions |
| `TRS(pos, rot, scale)` | Combined translation-rotation-scale |
| `TS(pos, scale)` | Translation + scale |
| `RS(rot, scale)` | Rotation + scale |
| `Orthographic(l, r, b, t, n, f)` | Orthographic projection |
| `Perspective(fov, aspect, near, far)` | Perspective projection |
| `PerspectiveInfiniteFarPlane(fov, aspect, near)` | Infinite far plane |
| `LookAt(camPos, target, up)` | View matrix |
| `OrthogonalInverse(m)` | Fast inverse for orthogonal matrices |
| `Inverse(m, *out)` | General inverse (returns bool) |
| `Transpose(m)` | Transpose |
| `MultiplyDirection(m, v)` | Transform direction (no translation) |
| `MultiplyPoint(m, v)` | Transform point (with translation) |
| `ProjectPoint(m, v)` | Perspective divide |
| `LeftMultiply(v, m)` | Row vector * matrix |
| `GetX(m)` / `GetY(m)` / `GetZ(m)` | Extract basis vectors |
| `GetTranslation(m)` / `GetScale(m)` / `GetRotation(m)` | Decompose TRS |
| `ToQuaternion(mat3)` | Convert rotation matrix to quaternion |
| `operator*` | Matrix multiply (mat*mat, mat*vec) |

```cpp
mat4 model = TRS(V3(0, 1, 0), AxisAngle(UP, DegToRad(90)), V3(1));
mat4 view = LookAt(V3(0, 5, 5), V3(0, 0, 0), UP);
mat4 proj = Perspective(DegToRad(60.0f), 16.0f/9.0f, 0.1f, 100.0f);
mat4 mvp = proj * view * model;
```

---

### geometry.h

**Planes**

| Function | Description |
|----------|-------------|
| `MakePlane(pt, normal)` | Construct plane from point + normal |
| `MakePlane(pt, rotation)` | Construct plane from point + quaternion |
| `ClosestPointOnPlane(point, plane)` | Nearest point on plane |
| `PlaneTest(plane, point)` | Signed distance (negative = behind) |
| `PlaneDistance(point, plane)` | Signed distance |
| `PlanePlaneIntersection(a, b, &pt, &dir)` | Line of intersection |
| `PlanePlanePlaneIntersection(a, b, c, &pt)` | Point where 3 planes meet |
| `PlaneSegmentIntersection(plane, a, b, &pt)` | Segment-plane intersection |
| `TransformPlane(transform, plane)` | Transform plane by matrix |
| `TestPointFrustum(p, planes, count)` | Frustum cull test |

**Rays**

| Function | Description |
|----------|-------------|
| `MakeRay(origin, direction)` | Construct ray |
| `PointAt(ray, t)` | Point along ray at parameter t |
| `RaycastPlane(point, normal, ray, &t)` | Ray-plane intersection |
| `RaycastAABB(min, max, origin, dir, &tMin)` | Ray-AABB intersection |
| `ClosestToRayAt(a, b)` | Parameter on ray a closest to ray b |
| `TransformRay(transform, ray)` | Transform ray |

**Shapes and Primitives**

| Function | Description |
|----------|-------------|
| `PointInTriangle(p, a, b, c)` | Point in CCW triangle |
| `SignedTriangleArea(a, b, c)` | Signed area of triangle |
| `PointInPolygon(points, count, p)` | Point in arbitrary polygon |
| `CentroidOfPolygon(points, count)` | Centroid of polygon |
| `SignedAreaOfPolygon(points, count)` | Signed area |
| `ShoelaceFormula(points, count)` | Signed area (shoelace) |
| `PointCircleTest(p, center, radius)` | Point in circle |
| `CircleCircleTest(centerA, rA, centerB, rB)` | Circle-circle overlap |
| `SphereSphereTest(a, rA, b, rB)` | Sphere-sphere overlap |
| `SegmentCircleIntersection(p0, p1, center, radius, &t)` | Segment-circle intersection |
| `TestPointAABB(p, min, max)` | Point in AABB |
| `TestAABBAABB(minA, maxA, minB, maxB, &dir)` | AABB overlap with MTV |
| `TriangulateConvexPolygon(vertCount, indices, &count)` | Fan triangulation |
| `ConstructBasis(normal, &X, &Y)` | Build tangent basis |
| `PointInFOV(apex, dir, fov, p)` | Frustum/FOV test |
| `PointsAreCollinear(a, b, c, eps)` | Collinearity test |
| `ProjectTo2D(normal, point)` / `ProjectTo3D(point, origin, X, Y)` | Dimension projection |

**Rect helpers**

```cpp
Rect r = MakeRect(V2(0), V2(5, 5));
bool hit = PointRectTest(r, V2(3, 3));
Rect world = GlobalRect(position, r);
```

---

### bezier.h

| Function | Description |
|----------|-------------|
| `BezierQuadratic(a, b, c, t)` | Quadratic bezier (scalar, vec2, vec3) |
| `BezierCubic(a, b, c, d, t)` | Cubic bezier (scalar, vec2, vec3) |
| `BezierCubicDerivative(a, b, c, d, t)` | First derivative |
| `BezierCubicSecondDerivative(a, b, c, d, t)` | Second derivative |
| `BezierCubicSplit(a, b, c, d, t, ...)` | Split curve at t into two sub-curves |
| `ArcLength(a, b, c, d, t)` | Arc length via Gauss quadrature |
| `ReparameterizeByArcLength(a, b, c, d, count, *samples)` | Build arc-length LUT |
| `ReparameterizedByArcLengthEvaluate(count, samples, s)` | Evaluate reparameterized t |
| `ApproximateCubicBezier(a, b, c, d, segments, *points)` | Sample curve into points |
| `ApproximateBezierLength(a, b, c, d, segments, *points)` | Approximate total length |
| `TValueFromX(a, b, c, d, x)` | Solve for t given x (monotonic easing) |

```cpp
vec2 p = BezierCubic(V2(0,0), V2(0.5f,1), V2(0.5f,0), V2(1,1), t);
```

---

### rand.h

Uses a global LCG state by default. Each function also has a variant taking `LCGState *state` for isolated generators.

| Function | Description |
|----------|-------------|
| `SeedRand(seed)` | Seed the global RNG |
| `Randi()` | Random uint32 |
| `Randi(upperLimit)` | Random uint32 in [0, upper) |
| `RandiRange(lower, upper)` | Random int in [lower, upper) |
| `Randf()` | Random float in [0, 1) |
| `RandfUpper(upperLimit)` | Random float in [0, upper) |
| `RandfRange(lower, upper)` | Random float in [lower, upper) |

```cpp
SeedRand(42);
float r = Randf();
int n = RandiRange(1, 7);
```

---

## Mosaic (Tile Grid)

The tile-based rendering system. `Mosaic` is a global `MosaicMem*` pointer, `Tiles` is a global `MTile*` to the tile array.

### Coordinate System

Tile `(0, 0)` is the **bottom-left** tile of the grid. Tile `x` increases to the **right**, tile `y` increases **upward** (`(0, 0)` = bottom-left, `(0, gridHeight - 1)` = top-left, `(gridWidth - 1, gridHeight - 1)` = top-right). Mouse coordinates returned by the mouse functions are grid coordinates in the same space.

### Types

| Type | Description |
|------|-------------|
| `MTile` | `{position: vec2i, color: vec4}` |
| `MosaicMem` | All mosaic state: grid dimensions, colors, tiles, text |

### Setup

| Function | Description |
|----------|-------------|
| `SetMosaicGridSize(w, h)` | Set grid dimensions (1-512). Resizes tile array and fits camera. |

### Tile Access

```cpp
MTile *t = GetTile(3, 5);        // by int coords (NULL if out of bounds)
MTile *t = GetTile(V2i(3, 5));   // by vec2i
MTile *t = GetTile(V2(3, 5));    // by vec2 (truncated to int)
GetTileBlock(x, y, w, h, tiles, &count); // rectangular region
```

### Coloring

| Function | Description |
|----------|-------------|
| `SetTileColor(x, y, color)` | Color a single tile |
| `SetTileColor(x, y, r, g, b)` | Color a single tile |
| `SetTileColor(vec2, ...)` | Color by world position |
| `SetBlockColor(x, y, w, h, color)` | Color a rectangular block |
| `ClearTiles(color)` | Fill entire grid with color |

```cpp
SetTileColor(3, 5, RED);
SetTileColor(V2(2, 2), 0.0f, 1.0f, 0.0f);
SetBlockColor(0, 0, 8, 8, RGBHex(3498db));
ClearTiles(BLACK);
```

### Mouse / Input

| Function | Description |
|----------|-------------|
| `GetMousePosition()` | Grid position of hovered tile as vec2i (-1,-1 if none) |
| `GetMousePositionX()` / `GetMousePositionY()` | Individual coords |
| `GetHoveredTile()` | MTile* under cursor (NULL if off-grid) |
| `TilePositionsOverlap(a, b)` | Check if two tile positions are the same |

```cpp
vec2i mouse = GetMousePosition();
if (Input->pressed[Input_MouseLeft]) {
    SetTileColor(mouse.x, mouse.y, RED);
}
```

### Sprites

| Function | Description |
|----------|-------------|
| `DrawSprite(position, sprite)` | Draw sprite pixels onto tiles (alpha=0 skips) |

```cpp
DrawSprite(V2(2, 2), mySprite);
```

### Text

| Function | Description |
|----------|-------------|
| `DrawTextTop(color, fmt, ...)` | Draw text at top of grid (printf-style) |
| `DrawTextTop(color, scale, fmt, ...)` | Same with custom scale |
| `DrawTextTile(pos, size, color, fmt, ...)` | Draw text at a tile position |
| `DrawTextTile(pos, size, color, centered, fmt, ...)` | Same with centering option |
| `PushText(fmt, ...)` | Push text at current screen-space cursor, advances line |
| `SetTextCursor(x, y)` | Set screen-space text cursor |

```cpp
DrawTextTop(WHITE, "Score: %d", score);
DrawTextTile(V2(3, 5), 0.3f, GREEN, true, "X");
PushText("Line 1");
PushText("Line 2");
```

### Grid Appearance

| Function | Description |
|----------|-------------|
| `ShowGrid()` / `HideGrid()` | Toggle grid lines |
| `SetMosaicGridColor(color)` | Grid line color |
| `SetMosaicScreenColor(color)` | Background clear color |
| `SetGridColor(color)` | Alias for SetMosaicGridColor |

---

## UI (Immediate Mode)

Immediate-mode GUI system. Requires a `UIManager` global state and `UIBegin`/style stack.

### Types

| Type | Description |
|------|-------------|
| `WidgetRect` | `{origin: vec2, size: vec2}` |
| `UIStyle` | Theme: button colors, font, textSize, spacing, columnGap |
| `UIManager` | Layout cursor, widget state, hover/press tracking, style stack |

### Functions

| Function | Description |
|----------|-------------|
| `WidgetID(name)` | Generate stable uint32 ID from string |
| `UIBegin(origin)` | Start UI frame at given screen position |
| `UIWindow(pos, size, color, texture)` | Draw background panel (texture can be NULL) |
| `UIButton(size, label)` | Clickable button. Returns true on click. |
| `UILabel(color, textSize, fmt, ...)` | Printf-style text label |
| `UIPushImage(size, texture)` | Display an image widget |
| `UINextColumn(width)` | Start new layout column |
| `GetNextWidgetBounds()` | Peek at next widget's rect without placing it |
| `UIPushStyle(style)` | Push style onto stack (max depth 32) |
| `UIPopStyle()` | Pop style stack |
| `UICurrentStyle()` | Get pointer to active style |

```cpp
UIBegin(V2(10, 10));
UIWindow(V2(0, 0), V2(200, 300), RGB(0.2f, 0.2f, 0.2f), NULL);
if (UIButton(V2(160, 30), "Click Me")) {
    // button was clicked
}
UILabel(WHITE, 16.0f, "Hello World");
UIPopStyle();
```

---

## Input

Data-only header. Input state is accessed via the global `Input` pointer.

### Enums

| Enum | Values |
|------|--------|
| `InputKeyboardDiscrete` | `Input_A`..`Input_Z`, `Input_0`..`Input_9`, `Input_Space`, `Input_Return`, `Input_Escape`, `Input_Shift`, `Input_Control`, `Input_Alt`, `Input_F1`..`Input_F24`, arrow keys, etc. |
| `InputMouseDiscrete` | `Input_MouseLeft`, `Input_MouseRight`, `Input_MouseMiddle` |
| `InputMouseAnalogue` | `Input_MousePositionX/Y`, `Input_MousePositionXNorm/YNorm`, `Input_MousePositionXOffset/YOffset`, `Input_ScrollDirection` |

### Structs

| Struct | Key Fields |
|--------|------------|
| `InputDevice` | `pressed[]`, `released[]`, `framesHeld[]`, `analogue[]`, `prevAnalogue[]` |
| `InputEvent` | `device`, `index`, `discreteValue`, `value` |
| `InputManager` | `devices`, `mousePos`, `mousePosWorld`, `mousePosNorm`, `mousePosNormSigned`, `inputChars`, `events` |

### Usage

```cpp
// Keyboard
if (Input->devices[0].pressed[Input_Space]) { }
if (Input->devices[0].released[Input_Escape]) { }
if (Input->devices[0].framesHeld[Input_W] > 0) { }

// Mouse
vec2 mouseWorld = Input->mousePosWorld;
if (Input->devices[1].pressed[Input_MouseLeft]) { }
float scroll = Input->devices[1].analogue[Input_ScrollDirection];
```

**Note**: Device index 0 is keyboard, index 1 is mouse. There are no public functions in this header -- all access is through struct fields on the global `Input` pointer.
