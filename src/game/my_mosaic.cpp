
// relationship between color space and image size? I don't think it really matters
// @TODO: sample from color space as if it's a 3x3 grid. Just use textures duh
// I don't really care about using all the colors in the color space (tho maybe that's
// kinda key to the algo since using all the colors in space is equivalent to filling the
// entire space supposedly)

#define For(var, max) for (int32 var = 0; var < (max); var++)
#define ForRev(var, max) for (int32 var = (max) - 1; var >= 0; var--)

MemoryArena arena = {};

struct Image {
    int32 width;
    int32 height;
    int32 capacity;
    vec4 *colors;
};

struct CanvasCell {
    bool filled;
    vec4 color;
};

struct Canvas {
    int32 width;
    int32 height;

    int32 filledCount;
    DynamicArray<CanvasCell> cells;

    Sprite sprite;
    DynamicArray<bool> pixelVisits;
};

inline vec2i IndexTo2D(int32 index, int32 maxX, int32 maxY) {
    vec2i result;
    result.y = index / maxX;
    result.x = index % maxX;
    return result;
}

inline int32 PositionToIndex(vec2i pos, int32 width, int32 height) {
    return pos.x + (pos.y * width);
}

int32 GetCellIndex(vec2i position, Canvas &canvas) {
    if (position.x < 0 || position.x >= canvas.width ||
        position.y < 0 || position.y >= canvas.height) {
        return -1;
    }
        
    return position.x + (position.y * canvas.width);
}

vec2i GetCellPosition(int32 index, Canvas &canvas) {
    vec2i result;
    result.y = index / canvas.height;
    result.x = index % canvas.width;
    return result;
}

struct Crawler {
    vec2i position;
    vec2i colorPosition;
    vec3i colorPosition_;

    DynamicArray<vec2i> positionStack;
    DynamicArray<vec2i> colorPositionStack;
};

struct GameMem {
    Crawler crawler;

    Canvas canvas;
};

GameMem game = {};

void MyMosaicInit() {
    SeedRand((int32)(int64)Game);
    
    SetMosaicGridSize(64, 64);

    AllocateMemoryArena(&arena, Megabytes(64));

    Canvas &canvas = game.canvas;
    canvas.width = Mosaic->gridWidth;
    canvas.height = Mosaic->gridHeight;
    canvas.cells = MakeDynamicArray<CanvasCell>(&arena, canvas.height * canvas.width);


    //LoadSprite(&canvas.sprite, "data/textures/sky_paint.png");
    LoadSprite(&canvas.sprite, "data/textures/gradient.png");

    canvas.pixelVisits = MakeDynamicArray<bool>(&arena, canvas.sprite.height * canvas.sprite.width);

    For (y, canvas.sprite.height) {
        For (x, canvas.sprite.width) {
            PushBack(&canvas.pixelVisits, false);
        }
    }


    For (i, canvas.cells.capacity) {
        PushBack(&canvas.cells, {});
    }

    Crawler &crawler = game.crawler;
    crawler.positionStack = MakeDynamicArray<vec2i>(&arena, canvas.cells.count);
    crawler.colorPositionStack = MakeDynamicArray<vec2i>(&arena, canvas.cells.count);

    crawler.position =
        V2i(RandfRange(0, Mosaic->gridWidth), RandfRange(0, Mosaic->gridHeight));

    // game.crawler.colorPosition.x = RandfRange(0, canvas.colorCube.size);
    // game.crawler.colorPosition.y = RandfRange(0, canvas.colorCube.size);
    // game.crawler.colorPosition.z = RandfRange(0, canvas.colorCube.size);

    game.crawler.colorPosition.x = RandfRange(0, canvas.sprite.width);
    game.crawler.colorPosition.y = RandfRange(0, canvas.sprite.height);

    // game.crawler.colorPosition.x = 100;
    // game.crawler.colorPosition.y = 100;
}

// @TODO:
// start by picking random color and position
// when we're picking a color we're looking for an unused color. So are we keeping a color
// stack for that also? 
// So we need to keep a stack of the next points to visit right?
// Do we pick a random direction each time? 

// @BUG: 

void UpdateCrawler(Crawler &crawler, Canvas &canvas) {
    int32 currIndex = GetCellIndex(crawler.position, canvas);

    {
        DynamicArray<int32> neighbors = MakeDynamicArray<int32>(&Game->frameMem, 4);

        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                if (x == 0 && y == 0) { continue; }
                if (x != 0 && y != 0) { continue; }
                
                int32 index = GetCellIndex(crawler.position + V2i(x, y), canvas);
                if (index >= 0) {
                    PushBack(&neighbors, index);
                }
            }
        }

        DynamicArray<int32> unoccupiedNeighbors = MakeDynamicArray<int32>(&Game->frameMem, 4);
        For (i, neighbors.count) {
            if (!canvas.cells[neighbors[i]].filled) {
                PushBack(&unoccupiedNeighbors, neighbors[i]);
            }
        }

        int32 newIndex = -1;
#if 0
        

        if (unoccupiedNeighbors.count > 0) {
            int r = RandfRange(0, unoccupiedNeighbors.count);
            newIndex = unoccupiedNeighbors[r];
        }
        else {
            int r = RandfRange(0, neighbors.count);
            newIndex = neighbors[r];
        }
#else

        // What if we just added them to the stack in a random order and still popped
        // from the back (do we have to pop from the front??)
        bool randomDirection = true;

        if (crawler.positionStack.count == 0) {
            randomDirection = true;
        }

        // @TODO: and it can't be already be in our stack!
        For (i, unoccupiedNeighbors.count) {

            vec2i pos = GetCellPosition(unoccupiedNeighbors[i], canvas);
            bool unique = true;

            For (j, crawler.positionStack.count) {
                if (crawler.positionStack[j] == pos) {
                    unique = false;
                    break;
                }
            }

            if (unique) {
                PushBack(&crawler.positionStack,
                         pos);
            }
        }

        if (randomDirection) {
            vec2i newPosition;
            newIndex = GetCellIndex(crawler.positionStack[(int32)RandfRange(0, crawler.positionStack.count)], canvas);
        }
        else {
            vec2i newPosition;
            if (PopFront(&crawler.positionStack, &newPosition)) {
                newIndex = GetCellIndex(newPosition, canvas);
            }
        }
#endif

        //Print("seed %d %d", defaultLCGState.seed, r);
        crawler.position = GetCellPosition(newIndex, canvas);
    }

    {
        DynamicArray<int32> neighbors = MakeDynamicArray<int32>(&Game->frameMem, 4);

        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                if (x == 0 && y == 0) { continue; }
                if (x != 0 && y != 0) { continue; }
                
                int32 index = GetCellIndex(crawler.colorPosition + V2i(x, y), canvas);
                if (index >= 0) {
                    PushBack(&neighbors, index);
                }
            }
        }

        DynamicArray<int32> unoccupiedNeighbors = MakeDynamicArray<int32>(&Game->frameMem, 4);
        For (i, neighbors.count) {
            if (!canvas.pixelVisits[neighbors[i]]) {
                PushBack(&unoccupiedNeighbors, neighbors[i]);
            }
        }

        int32 newIndex = -1;

        bool randomDirection = false;

        For (i, unoccupiedNeighbors.count) {

            vec2i pos = GetCellPosition(unoccupiedNeighbors[i], canvas);
            bool unique = true;

            For (j, crawler.colorPositionStack.count) {
                if (crawler.colorPositionStack[j] == pos) {
                    unique = false;
                    break;
                }
            }

            if (unique) {
                PushBack(&crawler.colorPositionStack,
                         pos);
            }
        }

        if (randomDirection) {
            newIndex =
                PositionToIndex(crawler.colorPositionStack[(int32)RandfRange(0, crawler.colorPositionStack.count)], canvas.sprite.width, canvas.sprite.height);
        }
        else {
            vec2i newPosition;
            if (PopFront(&crawler.colorPositionStack, &newPosition)) {
                newIndex = PositionToIndex(newPosition, canvas.sprite.width, canvas.sprite.height);
            }
        }

        crawler.colorPosition = IndexTo2D(newIndex, canvas.sprite.width, canvas.sprite.height);
    }


    int32 newIndex = GetCellIndex(crawler.position, canvas);
    if (!canvas.cells[newIndex].filled) {
        canvas.filledCount++;
        canvas.cells[newIndex].filled = true;


        int32 colorIndex = GetCellIndex(crawler.colorPosition, canvas);
        vec4 color = V4(canvas.sprite.data[(colorIndex * 4) + 0] / 255.0f,
                        canvas.sprite.data[(colorIndex * 4) + 1] / 255.0f,
                        canvas.sprite.data[(colorIndex * 4) + 2] / 255.0f,
                        1.0f);
        canvas.cells[newIndex].color = color;
    }
}

void MyMosaicUpdate() {
    // if (InputPressed(Keyboard, Input_Space)) {
    //     UpdateCrawler(game.crawler, game.canvas);
    // }

    For (i, 100) {
        UpdateCrawler(game.crawler, game.canvas);
    }

    Canvas &canvas = game.canvas;

    For (i, canvas.cells.count) {
        vec2i pos = GetCellPosition(i, canvas);
        SetTileColor(pos.x, pos.y, canvas.cells[i].color);
    }

    {
        Crawler &crawler = game.crawler;

        {
            DynamicArray<int32> neighbors = MakeDynamicArray<int32>(&Game->frameMem, 4);

            for (int y = -1; y <= 1; y++) {
                for (int x = -1; x <= 1; x++) {
                    if (x == 0 && y == 0) { continue; }
                    if (x != 0 && y != 0) { continue; }
                
                    int32 index = GetCellIndex(crawler.position + V2i(x, y), canvas);
                    if (index >= 0) {
                        PushBack(&neighbors, index);
                    }
                }
            }

            // SetTileColor(crawler.position.x,
            //              crawler.position.y,
            //              0, 1, 0);
            
            For (i, neighbors.count) {
                int32 index = neighbors[i];
                vec2i pos = GetCellPosition(index, canvas);

                //SetTileColor(pos.x, pos.y, 0, 0, 0.5f);
                
                //crawler.position = GetCellPosition(newIndex, canvas);
            }
        }
    }
}
