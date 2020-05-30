MosaicMem *Mosaic = NULL;
Tile *Tiles = NULL;
 

void ComputeGridSize(uint8 newWidth, uint8 newHeight) {
    Mosaic->gridWidth = Clamp(newWidth, 1, 255);
    Mosaic->gridHeight = Clamp(newHeight, 1, 255);

    free(Mosaic->tiles);
    
    Mosaic->tileCapacity = Mosaic->gridWidth * Mosaic->gridHeight;
    Mosaic->tiles = (Tile *)malloc(sizeof(Tile) * Mosaic->tileCapacity);

    memset(Mosaic->tiles, 0, Mosaic->tileCapacity * sizeof(Tile));

    Mosaic->tileSize = (9.0f - Mosaic->padding) / Mosaic->gridWidth;

    // @TODO: add the line sizes
    Mosaic->gridSize.x = Mosaic->tileSize * Mosaic->gridWidth;
    Mosaic->gridSize.y = Mosaic->tileSize * Mosaic->gridHeight;
    
    Mosaic->gridOrigin = V2(0) + V2(-Mosaic->gridSize.x * 0.5f, Mosaic->gridSize.y * 0.5f);

    Tile *tiles = Mosaic->tiles;
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            int32 index = (y * Mosaic->gridWidth) + x;

            Tile *tile = &tiles[index];

            tile->position = V2i(x, y);
        }
    }
}

void MosaicMyDataInit(MyData *myData) {
    
}

void MyInit();


int32 CellIndex(int32 x, int32 y) {
    return x + (y * Mosaic->gridWidth);
}

void MosaicInit(GameMemory *mem) {
    Mosaic = &mem->mosaic;

    Mosaic->gridWidth = 32;
    Mosaic->gridHeight = 32;

    Mosaic->tileCapacity = Mosaic->gridWidth * Mosaic->gridHeight;
    Mosaic->tiles = (Tile *)malloc(sizeof(Tile) * Mosaic->tileCapacity);

    memset(Mosaic->tiles, 0, Mosaic->tileCapacity * sizeof(Tile));

    MoveMouse(Game->screenWidth / 2.0f, Game->screenHeight / 2.0f);

    Mosaic->padding = 1.5f;

    AllocateRectBuffer(Mosaic->gridWidth * Mosaic->gridHeight, &Mosaic->rectBuffer);

    real32 screenAspect = 16.0f / 9.0f;
    real32 levelAspect = Mosaic->gridWidth / (Mosaic->gridHeight * 1.0f);

    Mosaic->tileSize = (9.0f - Mosaic->padding) / Mosaic->gridHeight;

    // Note: proportional to tileSize so the grid doesn't take up more room proportionally
    Mosaic->lineThickness = Mosaic->tileSize * 0.04f;

    // @TODO: add the line sizes
    Mosaic->gridSize.x = Mosaic->tileSize * Mosaic->gridWidth;
    Mosaic->gridSize.y = Mosaic->tileSize * Mosaic->gridHeight;
    
    Mosaic->gridOrigin = V2(0) + V2(-Mosaic->gridSize.x * 0.5f, Mosaic->gridSize.y * 0.5f);

    Mosaic->screenColor = V4(0.2f, 0.2f, 0.2f, 1.0f);
    Mosaic->boardColor = V4(0, 0, 0, 1.0f);
    Mosaic->lineColor = V4(0.8f, 0.8f, 0.8f, 1.0f);

    Mosaic->onlyDrawBorder = true;

    Tile *tiles = Mosaic->tiles;
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            int32 index = (y * Mosaic->gridWidth) + x;

            Tile *tile = &tiles[index];

            tile->position = V2i(x, y);
        }
    }

    MosaicMyDataInit(&Mosaic->myData);

    MyInit();

    Mosaic->cells = (bool *)malloc(sizeof(bool) * Mosaic->gridWidth * Mosaic->gridHeight);
    Mosaic->cellsCopy = (bool *)malloc(sizeof(bool) * Mosaic->gridWidth * Mosaic->gridHeight);
    Mosaic->occupiedState = (bool *)malloc(sizeof(bool) * Mosaic->gridWidth * Mosaic->gridHeight);

    for (int i = 0; i < Mosaic->gridWidth * Mosaic->gridHeight; i++) {
        real32 r = RandfRange(0.0f, 1.0f);

        if (r < 0.1f) {
            Mosaic->cells[i] = true;
            Mosaic->occupiedState[i] = true;
        }
        else {
            Mosaic->cells[i] = false;
        }
    }
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
        DrawRect(rowLineCenter, V2(Mosaic->gridSize.x * 0.5f, Mosaic->lineThickness), Mosaic->lineColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        if (x > 0 && x < Mosaic->gridWidth) { continue; }
        
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);
        DrawRect(colLineCenter, V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f), Mosaic->lineColor);
    }
}

void DrawGrid() {
    for (int y = 0; y < Mosaic->gridHeight + 1; y++) {

        vec2 rowLineCenter = Mosaic->gridOrigin + V2((Mosaic->gridSize.x * 0.5f), 0) + V2(0, -y * Mosaic->tileSize);
        DrawRect(rowLineCenter, V2(Mosaic->gridSize.x * 0.5f, Mosaic->lineThickness), Mosaic->lineColor);
        
    }

    for (int x = 0; x < Mosaic->gridWidth + 1; x++) {
        vec2 colLineCenter = Mosaic->gridOrigin + V2(0, (-Mosaic->gridSize.y * 0.5f)) + V2(x * Mosaic->tileSize, 0);
        DrawRect(colLineCenter, V2(Mosaic->lineThickness, Mosaic->gridSize.y * 0.5f), Mosaic->lineColor);
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
        DrawRect(V2(0), Mosaic->gridSize * 0.5f, Mosaic->boardColor);
    }

    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        Tile *tile = &tiles[i];

        DrawTile(tile->position, tile->color);
    }

    if (Mosaic->onlyDrawBorder) {
        DrawBorder();    
    }
    else {
        DrawGrid();        
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

int32 WrapCoordinate(int32 coord, int32 max) {
    if (coord > max) {
        coord = 0;
    }
    if (coord < 0) {
        coord = max;
    }

    return coord;
}

int32 CountNeighbors(bool *cells, int32 x, int32 y) {
    int32 count = 0;

    // implement wrapping!

    int32 left = WrapCoordinate(x - 1, Mosaic->gridWidth - 1);
    int32 right = WrapCoordinate(x + 1, Mosaic->gridWidth - 1);
    int32 down = WrapCoordinate(y + 1, Mosaic->gridHeight - 1);
    int32 up = WrapCoordinate(y - 1, Mosaic->gridHeight - 1);

    if (cells[CellIndex(left, y)]) {
        count++;
    }
    if (cells[CellIndex(left, up)]) {
        count++;
    }
    if (cells[CellIndex(x, up)]) {
        count++;
    }
    if (cells[CellIndex(right, up)]) {
        count++;
    }
    if (cells[CellIndex(right, y)]) {
        count++;
    }
    if (cells[CellIndex(right, down)]) {
        count++;
    }
    if (cells[CellIndex(x, down)]) {
        count++;
    }
    if (cells[CellIndex(left, down)]) {
        count++;
    }

    return count;
}

void UpdateCells() {
    for (int i = 0; i < Mosaic->gridHeight * Mosaic->gridWidth; i++) {
        Mosaic->cellsCopy[i] = Mosaic->cells[i];
    }
    
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            int32 cellIndex = CellIndex(x, y);

            int32 neighbors = CountNeighbors(Mosaic->cellsCopy, x, y);

            if (Mosaic->cells[cellIndex]) {
                if (neighbors < 2 || neighbors > 3) {
                    Mosaic->cells[cellIndex] = false;
                }
            }
            else {
                if (neighbors == 3) {
                    Mosaic->cells[cellIndex] = true;
                }
            }
        }
    }
}

void MosaicUpdate() {
    Tile *tiles = Mosaic->tiles;
    
    Tile* hoveredTile = Mosaic->hoveredTile;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *t = GetTile(x, y);

            int32 cellIndex = CellIndex(x, y);

            bool cell = Mosaic->cells[cellIndex];

            if (cell) {
                Mosaic->occupiedState[cellIndex] = true;
                t->color = V4(1, 1, 1, 1);    
            }
            else {
                if (Mosaic->occupiedState[cellIndex]) {
                    t->color = V4(0.0f, 0.3f, 0.6f, 1.0f);
                }
                else {
                    t->color = V4(0, 0, 0, 1);
                }
            }
        }
    }

    real32 timeSinceUpdate = Game->time - Mosaic->lastTimeUpdated;
    if (timeSinceUpdate > 0.2f) {
        Mosaic->lastTimeUpdated = Game->time;

        UpdateCells();
    }

    if (hoveredTile != NULL) {

        int32 cellIndex = CellIndex(hoveredTile->position.x, hoveredTile->position.y);

        if (InputHeld(Input, Input_MouseLeft)) {
            Mosaic->cells[cellIndex] = true;
        }
        
        hoveredTile->color = V4(1, 0, 0, 1);
    }
}
