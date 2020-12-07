
struct MyData {
    /*
      Put all your game specific data in here.
     */
};

MyData* Data = NULL;

void MyMosaicInit() {
    // Here we allocate the data defined in MyData so we have it on the first frame.
    Data = (MyData*)malloc(sizeof(MyData));
    memset(Data, 0, sizeof(MyData));

    SetMosaicGridSize(25, 16);
    // We have a global pointer to the MosaicMem data so you can set things like this.
    Mosaic->drawGrid = true;

    Mosaic->gridColor = RGB(0.2f, 0.2, 0.2f);

    // These color values are "normalized" meaning that max is 1 and min is 0
    Mosaic->screenColor = RGB(0.3f, 0.7f, 1.0f);

    // This writes a string to the log.txt file that gets written into the output directory
    Log("color r %f g %f b %f", Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b);
}


vec2 otherPaddle = V2(2.0f, 8.0f);
int32 otherPaddleDirect = 1;
real32 otherPaddleSpeed = 3.0f;
vec2 guyPosition = V2(12.0f, 5.0f);
vec2 obstaclePosition = V2(8.0f, 8.0f);
vec2 obstacleVelocity = V2(4.0f, 0.0f);

void MyMosaicUpdate() {
    Tile* tiles = Mosaic->tiles;

    real32 t = Game->time / 10.0f;
    real32 red = Lerp(0.0f, 1.0f, t);

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile* tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);
        }
    }

    int32 startX = 5;
    int32 startY = 5;

    int32 length = 3;
    int32 tall = 3;
    int32 score = 3;
    // This part of the for loops give us our columns
    for (int i = 0; i < length; i++) {

        // For each column tho we want to draw a number of tiles below.
        for (int y = 0; y < tall; y++) {
            Tile* tile = GetTile((i * 2) + startX, startY + y);

            tile->color = RGB(0.8f, 0.3f, 0.3f);

            Tile* scoreTile = GetTile(i + startX, startY + startY);
        }
    }
    
   


    vec2 prevPosition = guyPosition;
    vec2 hoveredPosition = obstaclePosition;

    if (InputPressed(Input, Input_Left)) {
        guyPosition.x -= 1;
    }
    if (InputPressed(Input, Input_Right)) {
        guyPosition.x += 1;
    }
    if (InputPressed(Input, Input_Down)) {
        guyPosition.y += 1;
    }
    if (InputPressed(Input, Input_Up)) {
        guyPosition.y -= 1;
    }
    if (InputPressed(Input, Input_MouseLeft)) {
        obstaclePosition.x = Mosaic->hoveredTile->position.x;
        obstaclePosition.y = Mosaic->hoveredTile->position.y;
    }

    if (guyPosition.y > Mosaic->gridHeight - 1) {
        guyPosition.y = Mosaic->gridHeight - 1;
    }
    if (guyPosition.y < 0) {
        guyPosition.y = 0;
    }
    if (guyPosition.x > Mosaic->gridWidth - 1) {
        guyPosition.x = Mosaic->gridWidth - 1;
    }
    if (otherPaddle.y > Mosaic->gridHeight - 1) {
        otherPaddle.y = Mosaic->gridHeight - 1;
    }
    if (otherPaddle.y < 0) {
        otherPaddle.y = 0;
    }


    obstaclePosition = obstaclePosition + obstacleVelocity * Game->deltaTime;

    if (obstaclePosition.x > Mosaic->gridWidth - 1) {
        obstacleVelocity.x *= -1;
        obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
        Mosaic->screenColor = RGB(1.0f, 0.0f, 0.0f);
    }
    if (obstaclePosition.x < 0) {
        obstacleVelocity.x *= -1;
        obstacleVelocity.y = RandfRange(4.0f, -4.0f);
        Mosaic->screenColor = RGB(0.0f, 0.0f, 1.0f);
    }
    if (obstaclePosition.y > Mosaic->gridHeight - 1) {
        obstacleVelocity.y *= -1;
        obstacleVelocity.x = RandfRange(-4.0f, 4.0f);
    }
    if (obstaclePosition.y < 0) {
        obstacleVelocity.y *= -1;
        obstacleVelocity.x = RandfRange(4.0f, -4.0f);
    }

    otherPaddle.y += otherPaddleDirect * otherPaddleSpeed * Game->deltaTime;
    
    if (otherPaddle.y < obstaclePosition.y) {
        otherPaddleDirect = 1;
    }
    if (otherPaddle.y > obstaclePosition.y) {
        otherPaddleDirect = -1;
    }




    Tile* paddle2Tile = GetTile(otherPaddle.x, otherPaddle.y);

    Tile* guyTile = GetTile(guyPosition.x, guyPosition.y);

    Tile* obstacleTile = GetTile(obstaclePosition.x, obstaclePosition.y);

    Tile* hoveredTile = GetTile(hoveredPosition.x, hoveredPosition.y);

    if (guyTile == obstacleTile) {
        guyPosition = prevPosition;

        guyTile = GetTile(guyPosition.x, guyPosition.y);
    }
    if (obstacleTile == Mosaic->hoveredTile) {
        obstaclePosition = hoveredPosition;

        obstacleTile = GetTile(obstaclePosition.x, obstaclePosition.y);
    }
    
    if (obstacleTile == paddle2Tile) {
        obstacleVelocity.x *= -1;
        obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
    }

    

    Tile* paddleTiles[15];

    paddleTiles[0] = GetTile(guyPosition.x, guyPosition.y);
    paddleTiles[1] = GetTile(guyPosition.x - 1, guyPosition.y - 2);
    paddleTiles[2] = GetTile(guyPosition.x - 1, guyPosition.y -1);
    paddleTiles[3] = GetTile(guyPosition.x - 1, guyPosition.y);
    paddleTiles[4] = GetTile(guyPosition.x - 1, guyPosition.y + 1);
    paddleTiles[5] = GetTile(guyPosition.x - 1, guyPosition.y - 2);
    paddleTiles[6] = GetTile(otherPaddle.x, otherPaddle.y - 2);
    paddleTiles[7] = GetTile(otherPaddle.x, otherPaddle.y - 1);
    paddleTiles[8] = GetTile(otherPaddle.x, otherPaddle.y + 1);
    paddleTiles[9] = GetTile(otherPaddle.x, otherPaddle.y + 2);


if (obstacleTile == paddleTiles[1]) {
        obstacleVelocity.x *= -1;
        obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
    }
if(obstacleTile == paddleTiles[2]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
}
if (obstacleTile == paddleTiles[3]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
}
if (obstacleTile == paddleTiles[4]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
}
if (obstacleTile == paddleTiles[5]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(-4.0f, 4.0f);
}
if (obstacleTile == paddleTiles[6]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(4.0f, -4.0f);
}
if (obstacleTile == paddleTiles[7]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(4.0f, -4.0f);
}
if (obstacleTile == paddleTiles[8]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(4.0f, -4.0f);
}
if (obstacleTile == paddleTiles[9]) {
    obstacleVelocity.x *= -1;
    obstacleVelocity.y = RandfRange(4.0f, -4.0f);
}


    int32 paddleIndex = 0;
    for (int y = -2; y < 3; y++) {
        vec2 tilePosition = guyPosition;
        tilePosition.y += y;

        for (int x = -1; x < 2; x++) {
            tilePosition.x = guyPosition.x + x;

            Tile* tile = GetTile(tilePosition.x, tilePosition.y);

            paddleTiles[paddleIndex] = tile;
            paddleIndex++;
        }
    }

    //Print("Starting to draw paddle");

    for (int y = -2; y < 3; y++) {
        vec2 tilePosition = guyPosition;
        tilePosition.y += y;

        for (int x = -1; x < 1; x++) {
            tilePosition.x = guyPosition.x + x;

            //Print("(%d %d) (%f %f)", x, y, tilePosition.x, tilePosition.y);

            Tile* tile = GetTile(tilePosition.x, tilePosition.y);
            if (tile) {
                tile->color = RGB(1.0f, 1.0f, 1.0f);
            }
        }
    }

    for (int y = -2; y < 3; y++) {
        vec2 tilePosition = otherPaddle;
        tilePosition.y += y;

        for (int x = -1; x < 1; x++) {
            tilePosition.x = otherPaddle.x + x;

            //Print("(%d %d) (%f %f)", x, y, tilePosition.x, tilePosition.y);

            Tile* tile = GetTile(tilePosition.x, tilePosition.y);
            if (tile) {
                tile->color = RGB(1.0f, 1.0f, 1.0f);
            }
        }
    }

    //Print("Stopping paddle");

    if (guyTile) {
        guyTile->color = RGB(0.6f, 0.6f, 0.6f);
    }
    if (paddle2Tile) {
        paddle2Tile->color = RGB(0.6f, 0.6f, 0.6f);
    }
    if (Mosaic->hoveredTile) {
        Mosaic->hoveredTile->color = RGB(1, 1, 1);
    }

    if (obstacleTile) {
        obstacleTile->color = RGB(.0f, 0.0f, 1.0f);
    }

}

#if 0
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

void Game1() {

    vec2 prevPosition = position;

    if (InputPressed(Input, Input_Left)) {
        shouldSwitchScene = true;
    }
}

void MyMosaicUpdate() {
    Tile *tiles = Mosaic->tiles;

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            Tile *tile = GetTile(x, y);
            tile->color = RGB(0.0f, 0.0f, 0.0f);
        }
    }
    
    SetTileColor(5, 4, 0.5, 0.5, 0.5);

}
#endif
