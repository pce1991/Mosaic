// To compile this code put it in src/game/my_game.cpp 
// then go to game_code.cpp and set #MOSAIC to 0 and #MY_GAME to 1
struct Wall {
    vec2 position;
};

struct Monster {
    vec2 position;
};

struct Player {
    vec2 position;
};

enum PlacerType {
    PlacerType_Wall,
    PlacerType_Monster,
    PlacerType_Player,
};

struct Level {
    DynamicArray<Wall> walls;
    DynamicArray<Monster> monsters;
    Player player;
};

Level World = {};

PlacerType ActivePlacer = PlacerType_Wall;

vec2 PalettePos = V2(20, 20);
vec2 PaletteSize = V2(250, 320);

void LoadLevel() {
    World.walls = MakeDynamicArray<Wall>(&Core->permanentArena, 64);
    World.monsters = MakeDynamicArray<Monster>(&Core->permanentArena, 64);
    World.player = Player{ V2(0.0f, 0.0f) };

    PushBack(&World.walls, Wall{ V2(-3.0f, 0.0f) });
    PushBack(&World.walls, Wall{ V2(-2.0f, 0.0f) });
    PushBack(&World.walls, Wall{ V2(-1.0f, 0.0f) });
    PushBack(&World.walls, Wall{ V2(0.0f, 0.0f) });
    PushBack(&World.walls, Wall{ V2(1.0f, 0.0f) });
    PushBack(&World.monsters, Monster{ V2(3.0f, 1.5f) });
    PushBack(&World.monsters, Monster{ V2(3.0f, -1.5f) });
}

void MyGameInit() {
    LoadLevel();
}

void PushPaletteWindow() {
    UIPushWindow(PalettePos, PaletteSize, V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);
    UILabel("editor");
    if (UIButton(180, "# walls")) {
        ActivePlacer = PlacerType_Wall;
    }
    if (UIButton(180, "G monsters")) {
        ActivePlacer = PlacerType_Monster;
    }
    if (UIButton(180, "P player")) {
        ActivePlacer = PlacerType_Player;
    }
    UILabel("placer: %s", ActivePlacer == PlacerType_Wall ? "#" : ActivePlacer == PlacerType_Monster ? "G" : "P");
    UILabel("click world to place");
    UIPopWindow();
}

void MyGameUpdate() {
    ClearColor(RGB(0.08f, 0.08f, 0.1f));

    vec2 mouseWorld = Input->mousePosWorld;

    UIBegin();

    PushPaletteWindow();

    Rect paletteRect = {};
    paletteRect.min = PalettePos;
    paletteRect.max = PalettePos + PaletteSize;

    if (InputPressed(Mouse, Input_MouseLeft) && !PointRectTest(paletteRect, UI->mousePos)) {
        if (ActivePlacer == PlacerType_Wall) {
            PushBack(&World.walls, Wall{ mouseWorld });
        }
        else if (ActivePlacer == PlacerType_Monster) {
            PushBack(&World.monsters, Monster{ mouseWorld });
        }
        else {
            World.player.position = mouseWorld;
        }
    }

    for (int32 i = 0; i < World.walls.count; i++) {
        DrawText(World.walls[i].position, 0.5f, RGB(0.9f, 0.9f, 0.3f), false, "#");
    }

    for (int32 i = 0; i < World.monsters.count; i++) {
        DrawText(World.monsters[i].position, 0.5f, RGB(0.3f, 0.9f, 0.3f), false, "G");
    }

    DrawText(World.player.position, 0.5f, RGB(0.3f, 0.7f, 1.0f), false, "P");

    DrawRect(mouseWorld, V2(0.25f, 0.25f), V4(1, 1, 1, 0.3f));
}
