
// example game structure

enum LevelName {
                LevelName_Invalid,
                LevelName_Level1,
                LevelName_Level2,
};

LevelName currLevel = LevelName_Level1;
LevelName nextLevel = LevelName_Invalid;

struct Player {
    int32 x;
    int32 y;
};

Player player = {};

struct Wall {
    int32 x;
    int32 y;
};

int32 wallCount = 0;
Wall walls[512];

void AddWall(int32 startX, int32 startY, int32 width, int32 height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Wall wall = {};
            wall.x = startX + x;
            wall.y = startY + y;

            walls[wallCount] = wall;
            wallCount++;
        }
    }
}

void GenerateLevel1() {
    AddWall(8, 3, 1, 7);
    AddWall(11, 6, 5, 1);

    int32 randX = RandiRange(0, 6);
    AddWall(randX, 12, 3, 3);
}

void GenerateLevel2() {

    int32 toAddCount = RandiRange(4, 8);

    for (int i = 0; i < toAddCount; i++) {
        int32 randX = RandiRange(1, 14);
        int32 randY = RandiRange(0, 15);

        int32 randWidth = RandiRange(2, 6);
        int32 randHeight = RandiRange(2, 4);
        
        AddWall(randX, randY, randWidth, randHeight);    
    }
    
    // AddWall(6, 0, 2, 7);
    // AddWall(6, 12, 2, 7);
}

void MyMosaicInit() {
    SetMosaicGridSize(16, 16);

    player.x = 5;
    player.y = 5;

    GenerateLevel1();
}

void UpdatePlayer() {
    int32 startX = player.x;
    int32 startY = player.y;


    
    if (InputPressed(Keyboard, Input_LeftArrow)) {
        player.x--;
    }
    if (InputPressed(Keyboard, Input_RightArrow)) {
        player.x++;
    }
    if (InputPressed(Keyboard, Input_UpArrow)) {
        player.y--;
    }
    if (InputPressed(Keyboard, Input_DownArrow)) {
        player.y++;
    }

    if (player.x < 0) {
        if (currLevel == LevelName_Level1) {
            nextLevel = LevelName_Level2;
        }
        if (currLevel == LevelName_Level2) {
            nextLevel = LevelName_Level1;
        }
    }

    // @TODO: check collisions

    for (int i = 0; i < wallCount; i++) {
        Wall wall = walls[i];

        if (TilePositionsOverlap(player.x, player.y, wall.x, wall.y)) {
            player.x = startX;
            player.y = startY;
        }
    }
}

void RenderWorld() {
    SetTileColor(player.x, player.y, 0.2f, 0.8f, 0.2f);

    // render the walls
    for (int i = 0; i < wallCount; i++) {
        Wall wall = walls[i];
        SetTileColor(wall.x, wall.y, 0.18f, 0.25f, 0.4f);
    }
}

void MyMosaicUpdate() {

    // update state
    UpdatePlayer();
    
    // render
    RenderWorld();

    // change level if necessary
    if (nextLevel != LevelName_Invalid) {
        currLevel = nextLevel;
        nextLevel = LevelName_Invalid;

        player.x = 0;

        // reset all the state you need to
        wallCount = 0;

        if (currLevel == LevelName_Level1) {
            GenerateLevel1();
        }
        if (currLevel == LevelName_Level2) {
            GenerateLevel2();
        }
    }
}

