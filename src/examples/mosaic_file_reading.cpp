

#if 0
int32 spotIndex = 0;
int32 spotCapacity = 0;
int32 width = 0;
int32 height = 0;

struct Spot {
    vec4 color;
    vec2i position;
};

Spot *spots = NULL;

void MyMosaicInit() {
    SetMosaicGridSize(19, 19);

    vec2i cursor = V2i(0, 0);

    FileHandle file;
    if (OpenFileForRead("data/file_reading_example.txt", &file)) {

        bool isFirstLine = true;
        bool widthMismatch = false;

        int32 widthOnFirstLine = 0;
        // First thing to do is read the file and find out the dimensions.
        // Then we can find out what the data in the file is.
        while (file.offset < file.size) {
            char c = ReadChar(&file);

            if (c == 13) {
                c = ReadChar(&file);
            }

            // Assuming data is square
            // @TODO: trigger an error if any line has more width than the top line.
            if (c == 10) {
                if (width < widthOnFirstLine) {
                    widthMismatch = true;
                    Print("Error on line %d, width mismatch", height);
                }
                
                height++;
                width = 0;
                
                isFirstLine = false;

            }
            else {
                width++;
                
                if (isFirstLine) {
                    widthOnFirstLine++;
                }

                if (width > widthOnFirstLine) {
                    widthMismatch = true;
                    Print("Error on line %d, width mismatch", height);
                }
            }
        }


        spotCapacity = widthOnFirstLine * height;

        spots = PushArray(&Game->permanentArena, Spot, spotCapacity);


        FileSeek(&file, 0);

        while (file.offset < file.size) {
            char c = ReadChar(&file);

            if (c == 13) {
                c = ReadChar(&file);
            }
            
            Print("%d %c", c, c);

            if (spotIndex < spotCapacity) {
                spots[spotIndex].position = cursor;
                
                if (c == '*') {
                    spots[spotIndex].color = RGB(0.9f, 0.5f, 0.4f);
                }
                if (c == '@') {
                    spots[spotIndex].color = RGB(0.3f, 0.8f, 0.5f);
                }

                if (c == 10) {
                    cursor.y++;
                    cursor.x = 0;
                }
                else {
                    spotIndex++;
                    cursor.x++;
                }
            }
        }

        CloseFile(&file);
    }
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    for (int i = 0; i < spotCapacity; i++) {
        SetTileColor(spots[i].position.x, spots[i].position.y, spots[i].color);
    }

    SetTileColor(9, 9, 1, 0, 0);
}

#else

struct Player {
    vec2i position;
};

struct Box {
    vec2i position;
};

struct Wall {
    vec2i position;
};

struct Pad {
    vec2i position;
};

struct Door {
    vec2i position;
    bool open;
    int32 padIndex;
};

struct Level {
    Player player;

    int32 wallCount;
    int32 wallCapacity;
    Wall *walls;

    int32 boxCount;
    int32 boxCapacity;
    Box *boxs;
};

struct MyGame {
    MemoryArena arena;

    Level *levels;

    Level *levelCurr;
};

MyGame game = {};

void MyMosaicInit() {
    SetMosaicGridSize(19, 19);

    AllocateMemoryArena(&game.arena, Megabytes(64));

    game.levels = PushArray(&game.arena, Level, 2);

    game.levelCurr = &game.levels[0];

    *game.levelCurr = {};
    game.levelCurr->wallCapacity = 256;
    game.levelCurr->walls = PushArray(&game.arena, Wall, game.levelCurr->wallCapacity);

    game.levelCurr->boxCapacity = 16;
    game.levelCurr->boxs = PushArray(&game.arena, Box, game.levelCurr->boxCapacity);

    Level *levelCurr = game.levelCurr;


    vec2i cursor = V2i(0, 0);
    FileHandle file;
    if (OpenFileForRead("data/sokoban_level.txt", &file, &Game->frameMem)) {

        while (file.offset < file.size) {
            char c = ReadChar(&file);

            if (c == 13) {
                c = ReadChar(&file);
            }

            if (c == '#') {
                Wall *wall = &levelCurr->walls[levelCurr->wallCount];
                wall->position = cursor;

                levelCurr->wallCount++;
            }

            if (c == '@') {
                levelCurr->player.position = cursor;
            }

            if (c == 'B') {
                Box *box = &levelCurr->boxs[levelCurr->boxCount];
                box->position = cursor;

                levelCurr->boxCount++;
            }

            if (c == 10) {
                cursor.y++;
                cursor.x = 0;
            }
            else {
                cursor.x++;
            }
        }

        CloseFile(&file);
    }
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);


    Level *levelCurr = game.levelCurr;

    for (int i = 0; i < levelCurr->wallCount; i++) {
        Wall *wall = &levelCurr->walls[i];

        SetTileColor(wall->position.x, wall->position.y, 0.6f, 0.3f, 0.1f);
    }

    for (int i = 0; i < levelCurr->boxCount; i++) {
        Box *box = &levelCurr->boxs[i];

        SetTileColor(box->position.x, box->position.y, 0.7f, 0.5f, 0.1f);
    }

    Player *player = &levelCurr->player;
    SetTileColor(player->position.x, player->position.y, 0.0f, 0.7f, 0.9f);
}
#endif
