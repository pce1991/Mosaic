
struct Player {
    vec2i position;
    int32 health;
};

struct Scorpion {
    vec2i position;
    int32 health;
    int32 strength;
};

struct Block {
    vec2 position;
    vec4 color;
};

struct Wall {
    vec2 position;
};

struct Switch {
    vec2 positon;
    bool active;
};

void InitPlayer(Player *p) {
    p->position = V2i(5, 5);
    p->health = 10;    
}

struct Level {
    vec2i dimensions;

    Player player;

    int32 scorpionCount;
    int32 scorpionCapacity;
    Scorpion *scorpions;
};

struct RPGMem {
    MemoryArena levelArena;

    Level level;
};

RPGMem rpg = {};

void MyMosaicInit() {
    SetMosaicGridSize(19, 19);
    HideGrid();
    SetGridColor(0.5f, 0.5f, 0.5f);
    SetMosaicScreenColor(0.35f, 0.2f, 0.2f);

    AllocateMemoryArena(&rpg.levelArena, Megabytes(16));

    rpg.level = {};

    InitPlayer(&rpg.level.player);

    Level *level = &rpg.level;

    level->scorpionCount = 0;
    level->scorpionCapacity = 20;
    // Get some memory for my scorpions.
    level->scorpions = PushSize(&rpg.levelArena, Scorpion, level->scorpionCapacity);

    level->scorpionCount = 4;

    for (int i = 0; i < level->scorpionCount; i++) {
        Scorpion *s = &level->scorpions[i];

        s->position = V2i(RandiRange(9, 19), RandiRange(0, 19));

        s->health = 4;
        s->strength = 3;
    }
}


// clamp should set n to be min if it's below min, and to max if it's above max
void Clamp(int32 *n, int32 min, int32 max) {
    if (*n < min) {
        *n = min;
    }
    else if (*n > max) {
        *n = max;
    }
}

void Clamp(real32 *n, real32 min, real32 max) {
    if (*n < min) {
        *n = min;
    }
    else if (*n > max) {
        *n = max;
    }
}


void UpdatePlayer(Player *player) {

    if (InputPressed(Input, Input_Right)) {
        player->position.x += 1;
    }
    if (InputPressed(Input, Input_Left)) {
        player->position.x -= 1;
    }
    if (InputPressed(Input, Input_Up)) {
        player->position.y -= 1;
    }
    if (InputPressed(Input, Input_Down)) {
        player->position.y += 1;
    }

    Clamp(&player->position.x, 0, 18);
    Clamp(&player->position.y, 0, 18);
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    Level *level = &rpg.level;

    Player *player = &level->player;

    UpdatePlayer(player);

    for (int i = 0; i < level->scorpionCount; i++) {
        Scorpion *s = &level->scorpions[i];

        if (s->position == player->position) {
            s->health -= 5;

            player->health -= s->strength;
        }
    }

    SetTileColor(player->position.x, player->position.y, 0.8f, 0.5f, 0.6f);
    
    for (int i = 0; i < level->scorpionCount; i++) {
        Scorpion *s = &level->scorpions[i];
        SetTileColor(s->position.x, s->position.y, 0.8f, 0.7f, 0.2f);
    }


    // Remove all th dead scorpions from the buffer and decrement the count.
    // We do this at the end of the frame so we dont ever have to worry about the count
    // changing during the updating of a frame.
    for (int i = level->scorpionCount - 1; i >= 0; i--) {
        Scorpion *s = &level->scorpions[i];

        if (s->health <= 0) {
            Scorpion end = level->scorpions[level->scorpionCount - 1];

            //*s = end; // could do this same thing with a dereference.
            level->scorpions[i] = end;

            level->scorpionCount--;
        }
    }

    DrawTextScreenPixel(&Game->monoFont, V2(800, 50), 32.0f, RGB(0.2f, 0.8f, 1.0f), true, "Sc count %d", level->scorpionCount);
}

