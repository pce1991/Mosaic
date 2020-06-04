
MosaicMem *Mosaic = NULL;
MyData *Data = NULL;

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
    myData->setBlocks = (bool *)malloc(sizeof(bool) * Mosaic->gridWidth * Mosaic->gridHeight);
    memset(myData->setBlocks, 0, sizeof(bool) * Mosaic->gridWidth * Mosaic->gridHeight);

    myData->pieceType = PieceType_ElbowRight;
}

void MosaicInit(GameMemory *mem) {
    Mosaic = &mem->mosaic;
    Data = &Mosaic->myData;

    Mosaic->gridWidth = 11;
    Mosaic->gridHeight = 24;

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
}

void RandomizeTiles() {
    Tile *tiles = Mosaic->tiles;
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            int32 index = (y * Mosaic->gridWidth) + x;

            Tile *tile = &tiles[index];

            tile->position = V2i(x, y);
        }
    }
    
    for (int i = 0; i < Mosaic->tileCapacity; i++) {
        Tile *tile = &tiles[i];

        //int32 r = RandiRange(0, 2);
        //int32 r = Randi();
        int32 r = RandfRange(0, 2.0f);

        vec4 color = V4(RandfRange(0, 2.0f), RandfRange(0, 2.0f), RandfRange(0, 2.0f), 1.0f);

        if (r < 0.5f) {
            tile->active = true;
        }
        else {
            tile->active = false;
        }

        tile->color = color;
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

        if (tile->active) {
            DrawTile(tile->position, tile->color);
        }
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
    Mosaic->hoveredTilePrev = Mosaic->hoveredTile;
    Mosaic->hoveredTile = GetHoveredTile();
}

void MosaicUpdate() {
    Tile *tiles = Mosaic->tiles;
    
    Tile* hoveredTile = Mosaic->hoveredTile;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *t = GetTile(x, y);
            t->color = V4(0, 0, 0, 1);
            t->active = true;
        }
    }

    if (hoveredTile != NULL) {
        hoveredTile->active = true;
        hoveredTile->color = V4(1, 1, 1, 1);
    }

}

