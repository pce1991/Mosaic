
#define EX_MOSAIC_CLEAN 0
#define MY_MOSAIC 1
#define EX_MOSAIC_BASIC 1
#define EX_MOSAIC_PERF 1
#define EX_MOSAIC_AUDIO 0
#define EX_MOSAIC_RANDOM_TILES 0

#define EX_MOSAIC_FILE_READ 1

#define EX_MOSAIC_TILE_RENDER 1

#define EX_MOSAIC_PARTICLES 0

#define EX_MOSAIC_MIA 0

#define EX_MOSAIC_LERP_COLORS 0

#define EX_MOSAIC_SPRITE 0

#define EX_MOSAIC_DYNAMIC_ARRAYS 0

#define EX_MOSAIC_PHYSICS 0

#define EX_MOSAIC_1 0
#define EX_MOSAIC_2 0

#define RUBE_GOLDBERG 0

#define EX_MOSAIC_VARIABLES 0

#define EX_MOSAIC_MOUSE_DRAWING 0

#define EX_BABY_DEMO 1

#define EX_MOSAIC_GRID 1



#if EX_MOSAIC_CLEAN
#include "examples/mosaic_clean.cpp"

#elif MY_MOSAIC
#include "my_mosaic.cpp"

#elif EX_MOSAIC_BASIC
#include "examples/mosaic_basic.cpp"

#elif EX_MOSAIC_PERF
#include "examples/mosaic_perf.cpp"

#elif EX_MOSAIC_FILE_READ
#include "examples/mosaic_file_reading.cpp"

#elif EX_MOSAIC_TILE_RENDER
#include "examples/mosaic_tile_renderer.cpp"

#elif EX_MOSAIC_RANDOM_TILES
#include "examples/mosaic_random_tiles.cpp"

#elif EX_MOSAIC_AUDIO
#include "examples/mosaic_audio.cpp"

#elif EX_MOSAIC_PARTICLES
#include "examples/mosaic_particles.cpp"

#elif EX_MOSAIC_SPRITE
#include "examples/mosaic_sprite.cpp"

#elif EX_MOSAIC_MIA
#include "examples/mosaic_mia.cpp"

#elif EX_BABY_DEMO
#include "examples/baby_demo.cpp"


#elif EX_MOSAIC_LERP_COLORS
#include "examples/lerp_colors.cpp"

#elif EX_MOSAIC_DYNAMIC_ARRAYS
#include "examples/mosaic_dynamic_arrays.cpp"

#elif EX_MOSAIC_PHYSICS
#include "examples/mosaic_physics.cpp"

#elif EX_MOSAIC_1
#include "examples/mosaic_1.cpp"

#elif EX_MOSAIC_VARIABLES
#include "examples/mosaic_variables.cpp"

#elif RUBE_GOLDBERG
#include "examples/rube_goldberg_machine.cpp"

#elif EX_MOSAIC_MOUSE_DRAWING
#include "examples/mosaic_mouse_drawing.cpp"

#elif EX_MOSAIC_GRID
#include "examples/mosaic_grid.cpp"

#elif EX_MOSAIC_2
#include "examples/mosaic_2.cpp"

//#elif MACRO_NAME
//#include "file_name.cpp"

#endif


// @NOTE: Some of this stuff is internal and you don't ever want it to change.
// Things like allocating the RectBuffer or calculating the levelAspect.
// Other things like setting the gridWidth can be customized.
void MyInit() {
    Game->myData = malloc(sizeof(MosaicMem));
    memset(Game->myData, 0, sizeof(MosaicMem));
    
    Mosaic = (MosaicMem *)Game->myData;

    MoveMouse(Game->screenWidth / 2.0f, Game->screenHeight / 2.0f);

    Mosaic->padding = 1.0f;

    SetMosaicGridSize(16, 16);

    Mosaic->screenColor = RGB(0.2f, 0.2f, 0.2f);
    Mosaic->gridColor = RGB(0.8f, 0.8f, 0.8f);

    MyMosaicInit();
}


// @BUG: this doesnt guarantee that it fits inside the camera if the height is bigger than the width! 
void SetMosaicGridSize(uint32 newWidth, uint32 newHeight) {
    Mosaic->gridWidth = Clamp(newWidth, 1, 255);
    Mosaic->gridHeight = Clamp(newHeight, 1, 255);

    if (Mosaic->tiles != NULL) {
        free(Mosaic->tiles);
    }
    
    Mosaic->tileCapacity = Mosaic->gridWidth * Mosaic->gridHeight;
    Mosaic->tiles = (MTile*)malloc(sizeof(MTile) * Mosaic->tileCapacity);

    memset(Mosaic->tiles, 0, Mosaic->tileCapacity * sizeof(MTile));

    real32 levelAspect = Mosaic->gridWidth / (Mosaic->gridHeight * 1.0f);
    real32 screenAspect = 16.0 / 9.0f;
    // @HARDCODED

    Mosaic->tileSize = 1;

    // @TODO: keep a dedicated place at the top for text?
    {
        Camera *cam = &Game->camera;

        if (levelAspect > screenAspect) {
            real32 size = Mosaic->gridWidth / (16.0f - Mosaic->padding);
            
            cam->width = 16.0f * size;
            cam->height = 9.0f * size;
        }
        else {
            real32 size = Mosaic->gridHeight / (9.0f - Mosaic->padding);
            
            cam->width = 16.0f * size;
            cam->height = 9.0f * size;
        }
        
        cam->type = CameraType_Orthographic;
        // cam->width = 16;
        // cam->height = 9;
        cam->projection = Orthographic(cam->width * -0.5f, cam->width * 0.5f,
                                       cam->height * -0.5f, cam->height * 0.5f,
                                       0.0, 100.0f);

        mat4 camWorld = TRS(Game->cameraPosition, Game->cameraRotation, V3(1));
        cam->view = OrthogonalInverse(camWorld);
    
        cam->viewProjection = cam->projection * cam->view;

    }

    Mosaic->lineThickness = Mosaic->tileSize * 0.04f;

    // @TODO: add the line sizes
    Mosaic->gridSize.x = Mosaic->tileSize * Mosaic->gridWidth;
    Mosaic->gridSize.y = Mosaic->tileSize * Mosaic->gridHeight;
    
    Mosaic->gridOrigin = V2(0) + V2(-Mosaic->gridSize.x * 0.5f, Mosaic->gridSize.y * 0.5f);

    //AllocateRectBuffer(Mosaic->gridWidth * Mosaic->gridHeight, &Mosaic->rectBuffer);

    MTile*tiles = Mosaic->tiles;
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            MTile*tile = GetTile(x, y);
            tile->position = V2i(x, y);
        }
    }
}

void SetMosaicGridColor(vec4 color) {
    Mosaic->gridColor = color;
}

void SetMosaicGridColor(real32 r, real32 g, real32 b) {
    Mosaic->gridColor = RGB(r, g, b);
}

void SetMosaicScreenColor(vec4 color) {
    Mosaic->screenColor = color;
}

void SetMosaicScreenColor(real32 r, real32 g, real32 b) {
    Mosaic->screenColor = RGB(r, g, b);
}

void SetGridColor(vec4 color) {
    Mosaic->gridColor = color;
}

void SetGridColor(real32 r, real32 g, real32 b) {
    Mosaic->gridColor = RGB(r, g, b);
}

void ShowGrid() {
    Mosaic->drawGrid = true;
}

void HideGrid() {
    Mosaic->drawGrid = false;
}


int32 CellIndex(int32 x, int32 y) {
    return x + (y * Mosaic->gridWidth);
}

vec2 GridPositionToWorldPosition(vec2i gridPosition) {
    vec2 worldPos = Mosaic->gridOrigin;
    real32 size = Mosaic->tileSize;
    worldPos.x += (size * gridPosition.x) + (size * 0.5f);
    worldPos.y += (-size * gridPosition.y) + (-size * 0.5f);
    return worldPos;
}

void DrawTile(vec2i position, vec4 color) {
    vec2 worldPos = GridPositionToWorldPosition(position);
    //DrawRect(worldPos, V2(Mosaic->tileSize * 0.5f), color);
    // Instancing
    DrawRect(&Game->rectBuffer, worldPos, V2(Mosaic->tileSize * 0.5f), color);
}

void DrawBorder() {
    for (int y = 0; y < Mosaic->gridHeight + 1; y++) {

        if (y > 0 && y < Mosaic->gridHeight) { continue; }
        vec2 rowLineCenter = Mosaic->gridOrigin + V2((Mosaic->gridSize.x * 0.5f), 0) + V2(0, -y * Mosaic->tileSize);
        DrawRect(rowLineCenter, V2(Mosaic->gridSize.x * 0.5f + (Mosaic->lineThickness), Mosaic->lineThickness), Mosaic->gridColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        if (x > 0 && x < Mosaic->gridWidth) { continue; }
        
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);
        DrawRect(colLineCenter, V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f + (Mosaic->lineThickness)), Mosaic->gridColor);
    }
}

void DrawGrid() {
    
    for (int y = 0; y < Mosaic->gridHeight + 1; y++) {

        vec2 rowLineCenter = Mosaic->gridOrigin + V2((Mosaic->gridSize.x * 0.5f), 0) + V2(0, -y * Mosaic->tileSize);

        vec2 scale = V2(Mosaic->gridSize.x * 0.5f, Mosaic->lineThickness);
        if (y == 0 || y == Mosaic->gridHeight) {
            scale = V2(Mosaic->gridSize.x * 0.5f + Mosaic->lineThickness, Mosaic->lineThickness);
        }
        
        DrawRect(rowLineCenter, scale, Mosaic->gridColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);

        vec2 scale = V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f);
        if (x == 0 || x == Mosaic->gridWidth) {
            scale = V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f + Mosaic->lineThickness);
        }
        
        DrawRect(colLineCenter, scale, Mosaic->gridColor);
    }
}

MTile*GetHoveredTile() {
    Camera *cam = &Game->camera;
    
    vec2 mousePos = Input->mousePosNormSigned;
    mousePos.x *= cam->width * 0.5f;
    mousePos.y *= cam->height * 0.5f;

    real32 xDistFromOrig = mousePos.x - Mosaic->gridOrigin.x;
    real32 yDistFromOrig = Mosaic->gridOrigin.y - mousePos.y;

    if (xDistFromOrig < 0 || yDistFromOrig < 0) { return NULL; }

    int32 xCoord = xDistFromOrig / Mosaic->tileSize;
    int32 yCoord = yDistFromOrig / Mosaic->tileSize;

    if (xCoord >= Mosaic->gridWidth || yCoord >= Mosaic->gridHeight) {
        return NULL;
    }

    int32 index = (yCoord * Mosaic->gridWidth) + xCoord;
    return &Mosaic->tiles[index];
}

inline MTile*GetTile(int32 x, int32 y) {
    if (x < 0 || x >= Mosaic->gridWidth) {
        return NULL;
    }
    if (y < 0 || y >= Mosaic->gridHeight) {
        return NULL;
    }
    
    // TODO: clamp these in case they're out of bounds, so we don't crash
    int32 index = (y * Mosaic->gridWidth) + x;
    return &Mosaic->tiles[index];
}

inline MTile*GetTile(vec2i pos) {
    return GetTile(pos.x, pos.y);
}

inline MTile*GetTile(vec2 pos) {
    return GetTile(pos.x, pos.y);
}

    
inline void GetTileBlock(int32 x, int32 y, int32 width, int32 height, MTile**tiles, int32 *tilesRetrieved) {
    for (int y_ = y; y_ < height; y_++) {
        for (int x_ = x; x_ < width; x_++) {
            MTile *t = GetTile(x_, y_);

            if (t) {
                tiles[*tilesRetrieved] = t;
                *tilesRetrieved += 1;
            }
        }
    }
}

void SetBlockColor(int32 x, int32 y, int32 width, int32 height, vec4 color) {
    vec2i bottomRight = V2i(x + width, y + height);
    
    for (int y_ = y; y_ < bottomRight.y; y_++) {
        if (y_ < 0 || y_ >= Mosaic->gridHeight) {
            continue;
        }
        
        for (int x_ = x; x_ < bottomRight.x; x_++) {
            if (x_ < 0 || x_ >= Mosaic->gridWidth) {
                continue;
            }
            
            MTile *t = GetTile(x_, y_);
            if (t) {
                t->color = color;
            }
        }
    }
}

void SetBlockColor(vec2 pos, int32 width, int32 height, vec4 color) {
    vec2i posInt = V2i(floorf(pos.x), floorf(pos.y));
    SetBlockColor(posInt.x, posInt.y, width, height, color);
}

void SetBlockColor(vec2i pos, int32 width, int32 height, vec4 color) {
    SetBlockColor(pos.x, pos.y, width, height, color);
}
    
// @BUG: broken
void GetTilesInLine(int32 x0, int32 y0, int32 x1, int32 y1) {
    int32 y = y0;
    r32 error = 0;

    real32 leftX = x0;
    real32 rightX = x1;

    // @TODO: handle all cooridinate cases for drawing top to bottom, bottom to top, etc

    real32 deltaX = GetTileCenter(x1 - x0);
    real32 deltaY = GetTileCenter(y1 - y0);

    real32 deltaError = Abs(deltaY / deltaX);

    for (int32 x = leftX; x < rightX; x++) {
        SetTileColor(x, y, 1, 1, 1);

        error += deltaError;
        if (error >= 0.5f) {
            y += Sign(y);
            error -= 1;
        }
    }
}

void ClearTiles(vec4 color) {
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            MTile*tile = GetTile(x, y);
            tile->color = color;
        }
    }
}

void ClearTiles(real32 r, real32 g, real32 b) {
    ClearTiles(RGB(r, g, b));
}

inline void SetTileColor(int32 x, int32 y, vec4 color) {
    MTile*t = GetTile(x, y);
    if (t) {
        t->color = color;
    }
}

inline void SetTileColor(int32 x, int32 y, real32 r, real32 g, real32 b) {
    MTile*t = GetTile(x, y);
    if (t) {
        t->color = RGB(r, g, b);
    }
}

inline void SetTileColor(vec2 position, real32 r, real32 g, real32 b) {
    MTile*t = GetTile(position);
    if (t) {
        t->color = RGB(r, g, b);
    }
}

inline void SetTileColor(vec2 position, vec4 color) {
    MTile*t = GetTile(position);
    if (t) {
        t->color = color;
    }
}

inline void DrawSprite(vec2 position, Sprite *sprite) {
    for (int y = 0; y < sprite->height; y++) {
        for (int x = 0; x < sprite->width; x++) {
            vec2 pos = position + V2(x, y);

            MTile*t = GetTile(pos);

            int32 pixel = (x * 4) + (y * sprite->width * 4);

            if (t == NULL) { continue; }

            // We don't support alpha blending, but you can use an alpha
            // of 0 to specify that a tile shouldn't be colored. 
            if (sprite->data[pixel + 3] == 0) {
                continue;
            }

            t->color.r = sprite->data[pixel] / 255.0f;
            t->color.g = sprite->data[pixel + 1] / 255.0f;
            t->color.b = sprite->data[pixel + 2] / 255.0f;
            t->color.a = sprite->data[pixel + 3] / 255.0f;
        }
    }
}


vec2i GetMousePosition() {
    if (Mosaic->hoveredTile) {
        return Mosaic->hoveredTile->position;
    }

    return V2i(-1, -1);
}

int32 GetMousePositionX() {
    if (Mosaic->hoveredTile) {
        return Mosaic->hoveredTile->position.x;
    }

    return -1;
}

int32 GetMousePositionY() {
    if (Mosaic->hoveredTile) {
        return Mosaic->hoveredTile->position.y;
    }

    return -1;
}

bool TilePositionsOverlap(vec2i a, vec2i b) {
    return a == b;
}

bool TilePositionsOverlap(int32 ax, int32 ay, int32 bx, int32 by) {
    return TilePositionsOverlap(V2i(ax, ay), V2i(bx, by));
}

bool TilePositionsOverlap(vec2 a, vec2 b) {
    vec2i a_ = V2i(a.x, a.y);
    vec2i b_ = V2i(b.x, b.y);

    return a_ == b_;
}

bool TilePositionsOverlap(real32 ax, real32 ay, real32 bx, real32 by) {
    return TilePositionsOverlap(V2i(ax, ay), V2i(bx, by));
}

real32 GetTileCenter(real32 n) {
    int32 i = (int32)n;
    return i + 0.5f;
}

void DrawTextTop(vec4 color, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 position = Mosaic->gridOrigin + V2(Mosaic->gridSize.x * 0.5f, 0.1f);
    DrawText(&Game->monoFont, position, 0.35f, color, true, str);

    va_end(args);
}

void DrawTextTop(vec4 color, real32 scale, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 position = Mosaic->gridOrigin + V2(Mosaic->gridSize.x * 0.5f, 0.1f);
    DrawText(&Game->monoFont, position, 0.35f * scale, color, true, str);

    va_end(args);
}

void DrawTextTile(vec2 pos, float32 size, vec4 color, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 floorPos = V2(floorf(pos.x), -floorf(pos.y));
    
    vec2 position = Mosaic->gridOrigin + floorPos + V2(0.0f, -1.0f);
    DrawText(&Game->monoFont, position, size, color, false, str);

    va_end(args);
}

void DrawTextTile(vec2 pos, float32 size, vec4 color, bool center, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    vec2 floorPos = V2(floorf(pos.x), -floorf(pos.y));
    
    vec2 position = Mosaic->gridOrigin + floorPos + V2(0.0f, -1.0f);
    DrawText(&Game->monoFont, position, size, color, center, str);

    va_end(args);
}

void PushText(const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);

    char str[GlyphBufferCapacity];
    vsnprintf(str, PRINT_MAX_BUFFER_LEN, fmt, args);

    MosaicText *text = &Mosaic->text;

    DrawTextScreen(&Game->monoFont, text->cursor, text->size, text->color, false, str);

    FontTable *font = &Game->monoFont;

    text->cursor.y += font->lineHeight * text->size;
}

void SetTextCursor(real32 x, real32 y) {
    MosaicText *text = &Mosaic->text;
    text->cursor.x = x;
    text->cursor.y = y;
}

void MosaicRender() {
    MTile*tiles = Mosaic->tiles;

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b, 1.0f);

    Mosaic->rectBuffer.count = 0;
    {
        vec2 pos = Mosaic->gridOrigin + V2(Mosaic->gridSize.x * 0.5f, -Mosaic->gridSize.y * 0.5f);
        DrawRect(pos, Mosaic->gridSize * 0.5f, V4(0, 0, 0, 1));
    }

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        MTile*tile = &tiles[i];

        DrawTile(tile->position, tile->color);
    }

    if (Mosaic->drawGrid) {
        DrawGrid();        
    }
    else if (Mosaic->drawBorder) {
        DrawBorder();
    }
}

// @NOTE: this is here so code can be inserted into MosaicUpdate in any order you want without
// messing up the internal state of the engine.
// Feel free to ignore this if you know what you're doing.
void MosaicUpdateInternal() {
    Tiles = Mosaic->tiles;
    
    Mosaic->hoveredTilePrev = Mosaic->hoveredTile;
    Mosaic->hoveredTile= GetHoveredTile();
    ClearTiles(V4(0));
}

// This function gets called by our game engine every frame.
void MyGameUpdate() {
    MosaicUpdateInternal();
    MyMosaicUpdate();
    MosaicRender();
}

