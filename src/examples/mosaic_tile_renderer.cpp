
// @TODO: start with a much simpler hardcoded example to illustrate the difficulties
// we'd have if we just didnt have these abstractions

const int32 tileWidth = 5;
const int32 tileSize = tileWidth * tileWidth;

// @TODO: call this a sprite instead. 
struct Tile {
    vec4 colors[tileSize];
};

int32 width = 16;
int32 height = 16;

vec2 pos = V2(0, 0);

Tile playerTile = {};

Tile grassTile = {};
Tile grassTile2 = {};

void MyMosaicInit() {
    SetMosaicGridSize(width * tileWidth, height * tileWidth);
    HideGrid();

    {
        for (int i = 0; i < tileSize; i++) {
            playerTile.colors[i] = V4(0);
        }
        
        vec4 color = RGB(0.7f, 0.0f, 0.2f);
        color.a = 1;
        
        playerTile.colors[1 + (4 * tileWidth)] = color;
        playerTile.colors[3 + (4 * tileWidth)] = color;

        playerTile.colors[1 + (3 * tileWidth)] = color;
        playerTile.colors[3 + (3 * tileWidth)] = color;

        playerTile.colors[1 + (2 * tileWidth)] = color;
        playerTile.colors[2 + (2 * tileWidth)] = color;
        playerTile.colors[3 + (2 * tileWidth)] = color;

        playerTile.colors[0 + (1 * tileWidth)] = color;
        playerTile.colors[1 + (1 * tileWidth)] = color;
        playerTile.colors[2 + (1 * tileWidth)] = color;
        playerTile.colors[3 + (1 * tileWidth)] = color;
        playerTile.colors[4 + (1 * tileWidth)] = color;

        playerTile.colors[1 + (0 * tileWidth)] = color;
        playerTile.colors[2 + (0 * tileWidth)] = color;
        playerTile.colors[3 + (0 * tileWidth)] = color;
    }

    for (int i = 0; i < tileSize; i++) {
        grassTile.colors[i] = RGB(0.1f, RandfRange(0.6f, 1.0f), RandfRange(0.2f, 0.4f));
    }

    for (int i = 0; i < tileSize; i++) {
        grassTile2.colors[i] = RGB(0.1f, RandfRange(0.6f, 1.0f), 0.2f);
    }
}

// This is used because we really have a grid within a grid.
// So this tells us which "subtile" we should be at if we want to draw our bigger-tile.
vec2i GetTilePosition(int32 x, int32 y) {
    return V2i(x * tileWidth, y * tileWidth);
}

void DrawTile(int32 posX, int32 posY, Tile *tile) {

    int32 index = 0;
    
    for (int y = posY; y < posY + tileWidth; y++) {
        for (int x = posX; x < posX + tileWidth; x++) {
            // @NOTE: doing this because we're overwriting whatever tile is there which
            // means we wouldnt wind up blending between that tile and nothing.
            if (tile->colors[index].a > 0) {
                SetTileColor(x, y, tile->colors[index]);
            }
            index++;
        }
    }
}

// If we just want to draw a block of one color.
void DrawBlock(int32 posX, int32 posY, vec4 color) {
    for (int y = posY; y < posY + tileWidth; y++) {
        for (int x = posX; x < posX + tileWidth; x++) {
            color.a = 0;
            SetTileColor(x, y, color);
        }
    }
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    real32 speed = 40;
    if (InputHeld(Input, Input_Left)) {
        pos.x += -speed * DeltaTime;
    }
    if (InputHeld(Input, Input_Right)) {
        pos.x += speed * DeltaTime;
    }
    if (InputHeld(Input, Input_Down)) {
        pos.y += speed * DeltaTime;
    }
    if (InputHeld(Input, Input_Up)) {
        pos.y += -speed * DeltaTime;
    }

    vec2i grassPosition = GetTilePosition(1, 0);
    DrawTile(grassPosition.x, grassPosition.y, &grassTile);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            vec2i grassPosition = GetTilePosition(x, y);
            if (x % 2 == 0 || y % 2 == 0) {
                DrawTile(grassPosition.x, grassPosition.y, &grassTile);
            }
            else {
                DrawTile(grassPosition.x, grassPosition.y, &grassTile2);    
            }
        }
    }
    
    //DrawTile(pos.x, pos.y, RGB(0.8f, 0.0f, 0.2f));
    DrawTile(pos.x, pos.y, &playerTile);
}
