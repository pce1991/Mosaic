
struct Tile {
    // This is kinda redundant because the tile can't move, but it's easier to have a tile
    // and know where it is than pass it's position around. 
    vec2i position;
    vec4 color;
};

// @NOTE: origin of the grid is top left so coordinate (0, 0) is top left.

enum PieceType {
     PieceType_Block,
     PieceType_Pipe,
     PieceType_ElbowLeft,
     PieceType_ElbowRight,
     PieceType_Z,
     PieceType_Count,
};

enum BlockState {
                 BlockState_Empty = 0,
                 BlockState_Goal = 1,
                 BlockState_Satisfied = 2,
                 BlockState_Unsatisfied = 4,
};

struct MyData {
    // @NOTE: this is where you can put in any data specific to your game.
    bool *setBlocks;
    

    vec2i piecePos;
    PieceType pieceType;
    int32 rotation;
    vec4 pieceColor;

    real32 timeLastMoved;

    // 3x3 grid, and we rotate aroudn that
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

    MyData myData;
};
