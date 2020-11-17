
struct MyData {
    /*
      Put all your game specific data in here.
     */
};

MyData *Data = NULL;

void MyMosaicInit() {
    // Here we allocate the data defined in MyData so we have it on the first frame.
    Data = (MyData *)malloc(sizeof(MyData));
    memset(Data, 0, sizeof(MyData));
    // I like to clear all my data to 0 and then manually set anything I don't want to start at 0.

    /*
      You can call SetMosaicGridSize(n, m) to set the grid size
      Look at mosaic.h for the MosaicMem struct to see various parameters you can set
     */

    SetMosaicGridSize(16, 16);

    // We have a global pointer to the MosaicMem data so you can set things like this.
    Mosaic->drawGrid = false;

    Mosaic->gridColor = RGB(0.2f, 0.2f, 0.2f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    //Mosaic->screenColor = RGB(0.3f, 0.7f, 1.0f);
    Mosaic->screenColor = RGB(0.2f, 0.2f, 0.2f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}

// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.

// Lerp between two buffers of color
// Equation to calculate index
// Distance function
// 

real32 StaticTimeSet = 0.0f;
vec4 colors[16 * 16];
void RandomStatic() {

    real32 timeSince = Game->time - StaticTimeSet;

    if (timeSince > 0.1f) {
        for (int y = 0; y < Mosaic->gridHeight; y++) {
            for (int x = 0; x < Mosaic->gridWidth; x++) {
                Tile *tile = GetTile(x, y);

                real32 r = RandfRange(0.0f, 1.0f);
                colors[x + (y * Mosaic->gridWidth)] = RGB(r, r, r);
            }
        }

        StaticTimeSet = Game->time;
    }
    
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);

            real32 r = RandfRange(0.0f, 1.0f);
            tile->color = colors[x + (y * Mosaic->gridWidth)];
        }
    }
}

void UVColors() {
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);

            real32 u = x / ((Mosaic->gridWidth - 1) * 1.0f);
            real32 v = y / ((Mosaic->gridHeight - 1) * 1.0f);
            
            tile->color = RGB(u, v, 0.0f);
        }
    }
}

void ChessPattern() {

    int32 height = 2;
    int32 row = -1;
    int32 col = -1;
    
    for (int y = 0; y < Mosaic->gridHeight; y++) {

        if (y % 2 == 0) {
            row++;
        }

        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            
            if (x % 2 == 0) {
                col++;    
            }

            if (row % 2 == 0) {
                if (col % 2 == 0) {
                    tile->color = RGB(1.0f, 1.0f, 1.0f);
                }
                else {
                    tile->color = RGB(0.0f, 0.0f, 0.0f);
                }
            }
            else {
                if (col % 2 == 0) {
                    tile->color = RGB(0.0f, 0.0f, 0.0f);
                }
                else {
                    tile->color = RGB(1.0f, 1.0f, 1.0f);
                }
            }
        }
    }
}


real32 radius = 1.0f;

#if 0
void CircleMove() {

    real32 timeInFunction = Game.time - timeFunctionChanged;

    vec2 center = V2(Mosaic->gridWidth / 2.0f,
                     Mosaic->gridHeight / 2.0f);
    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            
            tile->color = RGB(0.0f, 0.0f, 0.0f);

            real32 distance = Distance()
        }
    }
}
#endif

int32 function = 0;

real32 timeFunctionChanged = 0;


DynamicArray<vec2> wallPositions = {};
bool mazeStarted = false;

void MazeGame() {

    // This is so we only add things to the array for the first frame the
    // MazeGame() function is called.
    // Think of this like initializing the data at the beginning of the game.
    if (!mazeStarted) {
        // Initialize the game by filling in the wallPositions array
        vec2 wallPosition = V2(5, 5);
        PushBack(&wallPositions, wallPosition);
    
        wallPosition = V2(6, 5);
        PushBack(&wallPositions, wallPosition);

        wallPosition = V2(7, 5);
        PushBack(&wallPositions, wallPosition);

        wallPosition = V2(8, 5);
        PushBack(&wallPositions, wallPosition);

        mazeStarted = true;
    }


    // render the walls
    for (int i = 0; i < wallPositions.count; i++) {
        Tile *tile = GetTile(wallPositions[i].x, wallPositions[i].y);
        tile->color = RGB(0.5f, 0.5f, 0.5f);
    }

    // Check collisions with the wall:
    // loop over all the walls, get the tile and see if it equals the players tile
}


// character changes color when collision happens
// item disappears

vec2 position = V2(5, 0);
vec2 velocity = V2(0);
bool isActive = true;

vec2 playerPosition = V2(5, 8);
real32 timeCollidedWithItem = -1;

DynamicArray<vec2> positions = {};
bool addedPositions = false;


vec2 playerPosition = V2(5, 5);

DynamicArray<vec2> wallPositions;

void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);
        }
    }

    if (!addedPositions) {
        vec2 p = V2(5, 4);
        PushBack(&wallPositions, p);

        p = V2(6, 4);
        PushBack(&wallPositions, p);

        p = V2(7, 4);
        PushBack(&wallPositions, p);

        addedPositions = true;
    }

    Tile *guyTile = GetTile(playerPosition.x, playerPosition.y);

    vec2 wallPosition = V2(11, 7);

    Tile *wallTile = GetTile(wallPositions[i].x, wallPositions[i].y);
    
    if (guyTile == wallTile) {
        Print("Colliding!");
    }


    if (InputPressed(Input, Input_MouseLeft)) {
        
    }
    if (InputHeld(Input, Input_MouseLeft)) {
        
    }
    if (InputReleased(Input, Input_MouseLeft)) {
        
    }
    

#if 0
    velocity.y = 3;

    //position.x += velocity.x * Game->deltaTime;
    position.y += velocity.y * Game->deltaTime;

    Tile *item = GetTile(position.x, position.y);

    Tile *player = GetTile(playerPosition.x, playerPosition.y);

    if (item && isActive) {
        item->color = RGB(1, 0, 0);
    }

    if (player) {
        // Collision detection: when the position occupy same tile on the board
        // then you know that they're colliding.
        if (item == player) {
            timeCollidedWithItem = Game->time;
            //isActive = false;

            RemoveAtIndex();
        }

        // Default to blue unless we detect we need to override
        player->color = RGB(0, 0, 1);
        
        if (timeCollidedWithItem >= 0) {
            real32 timeSince = Game->time - timeCollidedWithItem;
            real32 duration = 0.5f;

            if (timeSince < duration) {
                player->color = Lerp(RGB(0, 1, 0), RGB(0, 0, 1), timeSince / duration);
            }
        }
    }

#endif
    // if (function == 0) {
    //     RandomStatic();
    // }
    // if (function == 1) {
    //     UVColors();
    // }
    // ....

    //ChessPattern();
    //Squares();

    //MazeGame();
}
