
struct Tile {
    // This is kinda redundant because the tile can't move, but it's easier to have a tile
    // and know where it is than pass it's position around. 
    vec2i position;
    vec4 color;
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
void SetMosaicGridSize(uint8 newWidth, uint8 newHeight);
