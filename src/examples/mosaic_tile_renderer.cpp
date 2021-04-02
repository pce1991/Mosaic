
const int32 tileWidth = 8;
const int32 tileSize = tileWidth * tileWidth;

// @TODO: call this a sprite instead. 
struct Tile {
    vec4 colors[tileSize];
};

int32 width = 12;
int32 height = 8;

vec2 pos = V2(0, 0);

Tile grassTile = {};
Tile grassTile2 = {};

void MyMosaicInit() {
    SetMosaicGridSize(width * tileWidth, height * tileWidth);
    HideGrid();

    for (int i = 0; i < tileSize; i++) {
        grassTile.colors[i] = RGB(0.1f, RandfRange(0.6f, 1.0f), RandfRange(0.2f, 0.4f));
    }

    for (int i = 0; i < tileSize; i++) {
        grassTile2.colors[i] = RGB(0.1f, RandfRange(0.6f, 1.0f), 0.2f);
    }
}

// This is used because we really have a grid within a grid. So this tells us which "subtile" we should
// be at if we want to draw our bigger-tile.
vec2i GetTilePosition(int32 x, int32 y) {
    return V2i(x * tileWidth, y * tileWidth);
}

void DrawTile(int32 posX, int32 posY, Tile *tile) {

    int32 index = 0;
    
    for (int y = posY; y < posY + tileWidth; y++) {
        for (int x = posX; x < posX + tileWidth; x++) {
            SetTileColor(x, y, tile->colors[index]);
            index++;
        }
    }
}

void DrawTile(int32 posX, int32 posY, vec4 color) {
    for (int y = posY; y < posY + tileWidth; y++) {
        for (int x = posX; x < posX + tileWidth; x++) {
            SetTileColor(x, y, color);
        }
    }
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    real32 speed = 50;
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
    
    DrawTile(pos.x, pos.y, RGB(0.8f, 0.0f, 0.2f));
}
