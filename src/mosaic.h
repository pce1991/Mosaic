
enum TileState {
     TileState_Default,
};

struct Tile {
    bool active;
    TileState state;

    // This is kinda redundant because the tile can't move, but it's easier to have a tile
    // and know where it is than pass it's position around. 
    vec2i position;
    vec4 color;
};

// @NOTE: origin of the grid is top left so coordinate (0, 0) is top left.

enum SceneID {
     SceneID_MoonPelt,
     SceneID_EarthSmash,
     SceneID_BoatTides,
     SceneID_Count,
};

struct Scene {
    SceneID id;
    bool inited;
    void *data;
};

struct MosaicMem {
    // @TODO: separate this stuff out into internal state that the user doesnt touch
    real32 lineThickness;
    real32 tileSize;
    // This is the size of the grid in world units
    vec2 gridSize;
    vec2 gridOrigin;
    real32 padding;

    
    vec4 screenColor;
    vec4 boardColor;
    vec4 lineColor;
    vec4 textColor;

    bool onlyDrawBorder;
    
    uint8 gridWidth;
    uint8 gridHeight;

    uint32 tileCapacity;
    Tile *tiles;

    Tile *hoveredTile;
    Tile *hoveredTilePrev;

    Scene scenes[SceneID_Count];
};

struct MoonCrater {
    vec2i offsetPos;
    real32 timeHit;
    real32 radius;
};

struct MoonPelt {
    vec2 moonPos;
    real32 moonDepth;
    int32 moonDir;

    DynamicArray<MoonCrater> craters;
};


struct BoatTides {
    vec2 moonPos;
    vec2 boatPos;

    int32 moonDir;

    real32 waveRate;
    real32 lastWaveTime;
};
