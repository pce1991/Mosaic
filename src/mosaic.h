
struct Tile {
    // This is kinda redundant because the tile can't move, but it's easier to have a tile
    // and know where it is than pass it's position around. 
    vec2i position;
    vec4 color;

    Sprite *sprite;
};

struct MosaicMem {
    // @TODO: separate this stuff out into internal state that the user doesnt touch
    real32 lineThickness;
    real32 tileSize;
    // This is the size of the grid in world units
    vec2 gridSize;
    vec2 gridOrigin;
    real32 padding;

    RectBuffer rectBuffer;
    
    vec4 screenColor;
    vec4 gridColor;
    vec4 textColor;

    bool drawBorder;
    bool drawGrid;
    
    uint8 gridWidth;
    uint8 gridHeight;

    uint32 tileCapacity;
    Tile *tiles;
    
    Tile *hoveredTile;
    Tile *hoveredTilePrev;

    void *myData;
};

MosaicMem *Mosaic = NULL;
Tile *Tiles = NULL;


Tile *GetTile(int32 x, int32 y);
Tile *GetTile(vec2i pos);
Tile *GetTile(vec2 pos);

void GetTileBlock(int32 x, int32 y, int32 width, int32 height, Tile **tiles, int32 *tilesRetrieved);


void SetTileColor(int32 x, int32 y, vec4 color);
void SetTileColor(int32 x, int32 y, real32 r, real32 g, real32 b);

void ClearTiles(vec4 color);
void ClearTiles(real32 r, real32 b, real32 g);


vec2i GetMousePosition();
int32 GetMousePositionX();
int32 GetMousePositionY();


void SetMosaicGridSize(uint32 newWidth, uint32 newHeight);

void SetMosaicGridColor(vec4 color);
void SetMosaicGridColor(real32 r, real32 g, real32 b);
void SetMosaicScreenColor(vec4 color);
void SetMosaicScreenColor(real32 r, real32 g, real32 b);

Tile *GetHoveredTile();

