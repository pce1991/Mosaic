
// @TODO: start with a much simpler hardcoded example to illustrate the difficulties
// we'd have if we just didnt have these abstractions

const int32 spriteWidth = 16;
const int32 spriteSize = spriteWidth * spriteWidth;

struct MySprite {
    vec4 colors[spriteSize];
};

const int32 width = 16;
const int32 height = 9;

MySprite grassSprite = {};
MySprite grassSprite2 = {};

MySprite guySprite = {};

int32 grassIndex[width * height];

vec2 position = V2(0, 0);


void LoadSprite(MySprite *sprite, char *path) {
    int32 x, y, n;
    uint8 *data = stbi_load(path, &x, &y, &n, 4);

    // @WARINNG: We assume that our MySprite data is big enough to store
    // however many pixels we read from our png file. If the png file is bigger
    // we will start accessing colors outside the bounds of our colors array.
    for (int i = 0; i < x * y; i++) {
        // 4 is the "stride" of the data
        // / 255.0f to normalize the colors on range 0 to 1
        sprite->colors[i].r = data[(i * 4) + 0] / 255.0f;
        sprite->colors[i].g = data[(i * 4) + 1] / 255.0f;
        sprite->colors[i].b = data[(i * 4) + 2] / 255.0f;
        sprite->colors[i].a = data[(i * 4) + 3] / 255.0f;
    }

    // free the data that stbi_load allocated.
    free(data);
}


void MyMosaicInit() {
    SetMosaicGridSize(width * spriteWidth, height * spriteWidth);
    HideGrid();

    for (int i = 0; i < spriteSize; i++) {
        grassSprite.colors[i].r = RandfRange(0.1, 0.2f);
        grassSprite.colors[i].g = RandfRange(0.6, 0.9f);
        grassSprite.colors[i].b = RandfRange(0.2, 0.3f);
        grassSprite.colors[i].a = 1.0f;
    }

    for (int i = 0; i < spriteSize; i++) {
        grassSprite2.colors[i].r = RandfRange(0.1, 0.2f);
        grassSprite2.colors[i].g = RandfRange(0.6, 0.9f);
        grassSprite2.colors[i].b = RandfRange(0.2, 0.3f);
        grassSprite2.colors[i].a = 1.0f;
    }

    for (int i = 0; i < width * height; i++) {
        real32 r = RandfRange(0.0f, 1.0f);
        if (r < 0.3) {
            grassIndex[i] = 0;
        }
        else {
            grassIndex[i] = 1;
        }
    }

    {
        for (int i = 0; i < spriteSize; i++) {
            guySprite.colors[i] = RGB(0.7f, 0.3f, 0.3f);
        }
    }

    LoadSprite(&guySprite, "data/galaga_ship.png");
}

void DrawSprite(int32 xPos, int32 yPos, MySprite *sprite) {
    for (int y = 0; y < spriteWidth; y++) {
        for (int x = 0; x < spriteWidth; x++) {
            int32 index = x + (y * spriteWidth);

            if (sprite->colors[index].a < 1.0f) {
                continue;
            }
            
            SetTileColor(x + xPos, y + yPos, sprite->colors[index]);
        }
    }    
}

void DrawSpriteGrid(int32 xGrid, int32 yGrid, MySprite *sprite) {
    for (int y = 0; y < spriteWidth; y++) {
        for (int x = 0; x < spriteWidth; x++) {
            int32 index = x + (y * spriteWidth);
            
            if (sprite->colors[index].a < 1.0f) {
                continue;
            }
            
            SetTileColor(x + (xGrid * spriteWidth), y + (yGrid * spriteWidth), sprite->colors[index]);
        }
    }    
}


void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    int32 gIndex = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            MySprite *grass;
            if (grassIndex[gIndex] == 0) {
                grass = &grassSprite;
            }
            else {
                grass = &grassSprite2;
            }
            
            DrawSpriteGrid(x, y, grass);

            gIndex++;
        }
    }

    if (InputHeld(Input, Input_Right)) {
        position.x += DeltaTime * 50;
    }
    if (InputHeld(Input, Input_Left)) {
        position.x -= DeltaTime * 50;
    }
    if (InputHeld(Input, Input_Down)) {
        position.y += DeltaTime * 50;
    }
    if (InputHeld(Input, Input_Up)) {
        position.y -= DeltaTime * 50;
    }

    DrawSprite(position.x, position.y, &guySprite);
    
}
