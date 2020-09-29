
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
    Mosaic->drawGrid = true;

    Mosaic->gridColor = RGB(0.2f, 0.2f, 0.2f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    Mosaic->screenColor = RGB(0.3f, 0.7f, 1.0f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}



vec2 guyPosition = V2(12.0f, 5.0f);

vec2 obstaclePosition = V2(8.0f, 5.0f);
vec2 obstacleVelocity = V2(4.0f, 0.0f);

// This is where you put the code you want to run every update.
// This function is called every frame, and its what tells us what colors to draw
// all the tiles at, along with all the other state changes in our game.
void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);
        }
    }

    vec2 prevPosition = guyPosition;

    if (InputPressed(Input, Input_Left)) {
        guyPosition.x -= 1;
    }
    if (InputPressed(Input, Input_Right)) {
        guyPosition.x += 1;
    }

    // You can add 2D vectors together, this will just add the x of velocity to the x of position
    // and the same for y.
    obstaclePosition = obstaclePosition + obstacleVelocity * Game->deltaTime;

    if (obstaclePosition.x > Mosaic->gridWidth - 1) {
        obstacleVelocity.x *= -1;
        // RandfRange just gives us a new number between the min and the max
        obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
    }
    // TODO: set this up for other sides of the screen

    // After we make our move, check to see if we collide with anything,
    // and if we do collide, then go ahead and fix our position.

    // To handle the fact that our positions are real numbers, and that
    // means they might be close to equal but not actually equal,
    // we're going use the fact that everything is a Tile to make collisions
    // easier to detect.
    
    // Remember that this might give us a NULL pointer,
    // meaning that there is no tile to be found at this position.
    Tile *guyTile = GetTile(guyPosition.x, guyPosition.y);
    
    Tile *obstacleTile = GetTile(obstaclePosition.x, obstaclePosition.y);

    // 
    if (guyTile == obstacleTile) {
        guyPosition = prevPosition;
        // our position is now (7, 5). Have we gotten the tile at position (7, 5)?
        // guyPosition was (8, 5) when we called GetTile() above.
        // guyTile is still pointing the tile at (8, 5), which means we need to update it
        // since that is definitely not the tile that we want to color.
        // Now that we updated the position we want to update what tile we're talking about
        // because the tile we had was for our invalid position.
        guyTile = GetTile(guyPosition.x, guyPosition.y);
    }
    

    if (obstacleTile) {
        obstacleTile-> color = RGB(1.0f, 0.0f, 0.0f);
    }


    if (guyTile) {
        guyTile->color = RGB(0.0f, 0.6f, 0.2f);
    }
}
