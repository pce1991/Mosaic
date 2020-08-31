
#define EX_MOSAIC_CLEAN 0
#define EX_MOSAIC_BASIC 1
#define EX_MOSAIC_AUDIO 0
#define EX_MOSAIC_RANDOM_TILES 0

#define EX_MOSAIC_WEEK_1 0

#if EX_MOSAIC_CLEAN
#include "examples/mosaic_clean.cpp"

#elif EX_MOSAIC_BASIC
#include "examples/mosaic_basic.cpp"

#elif EX_MOSAIC_RANDOM_TILES
#include "examples/mosaic_random_tiles.cpp"

#elif EX_MOSAIC_AUDIO
#include "examples/mosaic_audio.cpp"

#elif EX_MOSAIC_WEEK_1
#include "examples/mosaic_week1.cpp"

//#elif MACRO_NAME
//#include "file_name.cpp"

#endif

// @BUG: this doesnt guarantee that it fits inside the camera if the height is bigger than the width! 
void SetMosaicGridSize(uint8 newWidth, uint8 newHeight) {
    Mosaic->gridWidth = Clamp(newWidth, 1, 255);
    Mosaic->gridHeight = Clamp(newHeight, 1, 255);

    if (Mosaic->tiles != NULL) {
        free(Mosaic->tiles);
    }
    
    Mosaic->tileCapacity = Mosaic->gridWidth * Mosaic->gridHeight;
    Mosaic->tiles = (Tile *)malloc(sizeof(Tile) * Mosaic->tileCapacity);

    memset(Mosaic->tiles, 0, Mosaic->tileCapacity * sizeof(Tile));

    Mosaic->tileSize = (9.0f - Mosaic->padding) / Mosaic->gridWidth;
    Mosaic->lineThickness = Mosaic->tileSize * 0.04f;

    // @TODO: add the line sizes
    Mosaic->gridSize.x = Mosaic->tileSize * Mosaic->gridWidth;
    Mosaic->gridSize.y = Mosaic->tileSize * Mosaic->gridHeight;
    
    Mosaic->gridOrigin = V2(0) + V2(-Mosaic->gridSize.x * 0.5f, Mosaic->gridSize.y * 0.5f);

    AllocateRectBuffer(Mosaic->gridWidth * Mosaic->gridHeight, &Mosaic->rectBuffer);

    Tile *tiles = Mosaic->tiles;
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->position = V2i(x, y);
        }
    }
}


int32 CellIndex(int32 x, int32 y) {
    return x + (y * Mosaic->gridWidth);
}

// @NOTE: Some of this stuff is internal and you don't ever want it to change.
// Things like allocating the RectBuffer or calculating the levelAspect.
// Other things like setting the gridWidth can be customized.
void MyInit() {
    Game->myData = malloc(sizeof(MosaicMem));
    memset(Game->myData, 0, sizeof(MosaicMem));
    
    Mosaic = (MosaicMem *)Game->myData;

    MoveMouse(Game->screenWidth / 2.0f, Game->screenHeight / 2.0f);

    Mosaic->padding = 1.5f;

    SetMosaicGridSize(16, 16);

    Mosaic->screenColor = V4(0.2f, 0.2f, 0.2f, 1.0f);
    Mosaic->gridColor = V4(0.8f, 0.8f, 0.8f, 1.0f);

    MyMosaicInit();
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
    DrawRect(worldPos, V2(Mosaic->tileSize * 0.5f), color);
    // Instancing
    //DrawRect(&Mosaic->rectBuffer, worldPos, V2(Mosaic->tileSize * 0.5f), color);
}

void DrawBorder() {
    for (int y = 0; y < Mosaic->gridHeight + 1; y++) {

        if (y > 0 && y < Mosaic->gridHeight) { continue; }
        vec2 rowLineCenter = Mosaic->gridOrigin + V2((Mosaic->gridSize.x * 0.5f), 0) + V2(0, -y * Mosaic->tileSize);
        DrawRect(rowLineCenter, V2(Mosaic->gridSize.x * 0.5f, Mosaic->lineThickness), Mosaic->gridColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        if (x > 0 && x < Mosaic->gridWidth) { continue; }
        
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);
        DrawRect(colLineCenter, V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f), Mosaic->gridColor);
    }
}

void DrawGrid() {
    for (int y = 0; y < Mosaic->gridHeight + 1; y++) {

        vec2 rowLineCenter = Mosaic->gridOrigin + V2((Mosaic->gridSize.x * 0.5f), 0) + V2(0, -y * Mosaic->tileSize);
        DrawRect(rowLineCenter, V2(Mosaic->gridSize.x * 0.5f, Mosaic->lineThickness), Mosaic->gridColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);
        DrawRect(colLineCenter, V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f), Mosaic->gridColor);
    }
}

Tile *GetHoveredTile() {
    vec2 mousePos = Input->mousePosNormSigned;
    mousePos.x *= 8;
    mousePos.y *= 4.5f;

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

Tile *GetTile(int32 x, int32 y) {
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

Tile *GetTile(vec2i pos) {
    return GetTile(pos.x, pos.y);
}

void GetTileBlock(int32 x, int32 y, int32 width, int32 height, Tile **tiles, int32 *tilesRetrieved) {
    for (int y_ = y; y < width; y_++) {
        for (int x_ = x; x < width; x++) {
            Tile *t = GetTile(x_, y_);
            if (t) {
                tiles[*tilesRetrieved] = t;
                *tilesRetrieved += 1;
            }
        }
    }
}


void MosaicRender() {
    Tile *tiles = Mosaic->tiles;
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b, 1.0f);
    Mosaic->rectBuffer.count = 0;
    {
        DrawRect(V2(0), Mosaic->gridSize * 0.5f, V4(0, 0, 0, 1));
    }

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        Tile *tile = &tiles[i];

        DrawTile(tile->position, tile->color);
    }

    if (Mosaic->drawGrid) {
        DrawGrid();        
    }
    else if (Mosaic->drawBorder) {
        DrawBorder();
    }

    //Instancing
    //RenderRectBuffer(&Mosaic->rectBuffer);
}

// @NOTE: this is here so code can be inserted into MosaicUpdate in any order you want without
// messing up the internal state of the engine.
// Feel free to ignore this if you know what you're doing.
void MosaicUpdateInternal() {
    Tiles = Mosaic->tiles;
    
    Mosaic->hoveredTilePrev = Mosaic->hoveredTile;
    Mosaic->hoveredTile = GetHoveredTile();
}

// This function gets called by our game engine every frame.
void MyGameUpdate() {
    MosaicUpdateInternal();
    MyMosaicUpdate();
    MosaicRender();
}

