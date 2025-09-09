
struct MTile{
    // This is kinda redundant because the tile can't move, but it's easier to have a tile
    // and know where it is than pass it's position around. 
    vec2i position;
    vec4 color;
};

struct MosaicText {
    vec2 cursor;

    float32 size;
    vec4 color;
};

struct MosaicMem {
    // @TODO: separate this stuff out into internal state that the user doesnt touch
    float32 lineThickness;
    float32 tileSize;
    // This is the size of the grid in world units
    vec2 gridSize;
    vec2 gridOrigin;
    float32 padding;

    RectBuffer rectBuffer;
    
    vec4 screenColor;
    vec4 gridColor;
    vec4 textColor;

    bool drawBorder;
    bool drawGrid;
    
    uint8 gridWidth;
    uint8 gridHeight;

    uint32 tileCapacity;
    MTile*tiles;
    
    MTile*hoveredTile;
    MTile*hoveredTilePrev;
    
    MosaicText text;

    void *myData;
};

MosaicMem *Mosaic = NULL;
MTile*Tiles = NULL;


MTile*GetTile(int32 x, int32 y);
MTile*GetTile(vec2i pos);
MTile*GetTile(vec2 pos);

void GetTileBlock(int32 x, int32 y, int32 width, int32 height, MTile**tiles, int32 *tilesRetrieved);

void GetTilesInLine(int32 x0, int32 y0, int32 x1, int32 y1);


void SetTileColor(int32 x, int32 y, vec4 color);
void SetTileColor(int32 x, int32 y, float32 r, float32 g, float32 b);

void SetTileColor(vec2 position, float32 r, float32 g, float32 b);
void SetTileColor(vec2 position, vec4 color);

void SetTileColor(vec2i position, float32 r, float32 g, float32 b);
void SetTileColor(vec2i position, vec4 color);

void SetBlockColor(int32 x, int32 y, int32 width, int32 height, vec4 color);
void SetBlockColor(vec2 pos, int32 width, int32 height, vec4 color);
void SetBlockColor(vec2i pos, int32 width, int32 height, vec4 color);

void ClearTiles(vec4 color);
void ClearTiles(float32 r, float32 g, float32 b);

void DrawSprite(vec2 position, Sprite *sprite);
    
vec2i GetMousePosition();
int32 GetMousePositionX();
int32 GetMousePositionY();


void SetMosaicGridSize(uint32 newWidth, uint32 newHeight);

void SetMosaicGridColor(vec4 color);
void SetMosaicGridColor(float32 r, float32 g, float32 b);
void SetMosaicScreenColor(vec4 color);
void SetMosaicScreenColor(float32 r, float32 g, float32 b);

// Turn this into a "ShowGrid(true/false)"
void ShowGrid();
void HideGrid();

void SetGridColor(vec4 color);
void SetGridColor(float32 r, float32 g, float32 b);

bool TilePositionsOverlap(vec2 a, vec2 b);
bool TilePositionsOverlap(vec2i a, vec2i b);
bool TilePositionsOverlap(float32 ax, float32 ay, float32 bx, float32 by);
bool TilePositionsOverlap(int32 ax, int32 ay, int32 bx, int32 by);

float32 GetTileCenter(float32 n);

void DrawTextTop(vec4 color, const char *fmt, ...);
void DrawTextTop(vec4 color, float32 scale, const char *fmt, ...);

// The origin of the text will be at the bottom left of the tile position you give.
void DrawTextTile(vec2 position, float32 size, vec4 color, const char *fmt, ...);
void DrawTextTile(vec2 pos, float32 size, vec4 color, bool centered, const char *fmt, ...);

void PushText(const char *fmt, ...);

MTile*GetHoveredTile();

