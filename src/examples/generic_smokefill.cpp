
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

struct ColorCube {
    int32 size;
    DynamicArray<Image> images;
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

    ColorCube colorCube;
};

inline vec3i IndexTo3D(int32 index, int32  maxX, int32  maxY, int32  maxZ) {
    vec3i result;
    result.z = index / (maxX * maxY);
    index -= result.z * maxX * maxY;
    result.y = index / maxX;
    result.x = index % maxX;
    return result;
}

inline int32 PositionToIndex(vec3i index3D, int32  width, int32  height) {
    // @NOTE: these too lines are equivalent, but the bottom one has factored out the width
    //return index3D.x + (width * index3D.y) + (index3D.z * width * height);
    return index3D.x + width * (index3D.y + height * index3D.z);
}

int32 GetColorIndex(vec3i position, ColorCube &cube) {
    return PositionToIndex(position, cube.size, cube.size);
}

vec3i GetColorPosition(int32 index, ColorCube &cube) {
    return IndexTo3D(index, cube.size, cube.size, cube.size);    
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
    vec3i colorPosition;

    DynamicArray<vec2i> positionStack;
    DynamicArray<vec3i> colorPositionStack;
};

struct GameMem {
    Crawler crawler;

    Canvas canvas;
};

GameMem game = {};

void MyMosaicInit() {
    SeedRand((int32)(int64)Game);
    
    SetMosaicGridSize(256, 256);

    AllocateMemoryArena(&arena, Megabytes(64));

    Canvas &canvas = game.canvas;
    canvas.width = Mosaic->gridWidth;
    canvas.height = Mosaic->gridHeight;
    canvas.cells = MakeDynamicArray<CanvasCell>(&arena, canvas.height * canvas.width);

    {
        ColorCube &colorCube = canvas.colorCube;
        int32 size = 16;
        colorCube.size = size;

        colorCube.images = MakeDynamicArray<Image>(&arena, colorCube.size);
        
        // @TODO: generate colors uisng HSV somehow so we change over depth
        // vec4 minColor = V4(0.7f, 0.4f, 0.3f, 1.0f);
        // vec4 maxColor = V4(0.9f, 0.7f, 0.1, 1.0f);

        vec4 minColor = V4(0.0f, 0.0f, 0.0f, 1.0f);
        vec4 maxColor = V4(1.0f, 1.0f, 1.0f, 1.0f);

        // vec4 minHSV = V4(0.0f, 0.0f, 0.0f, 1.0f);
        // vec4 maxHSV = V4(360.0f, 1.0f, 1.0f, 1.0f);

        vec4 minHSV = V4(200.0f, 0.4f, 0.2f, 1.0f);
        vec4 maxHSV = V4(250.0f, 1.0f, 1.0f, 1.0f);

        bool useHSV = true;

        For (z, colorCube.size) {
            real32 zT = z / (colorCube.size * 1.0f);
            
            Image &image = *PushBackPtr(&colorCube.images);
            image.width = size;
            image.height = size;
            image.capacity = image.height * image.width;

            image.colors = PushArray(&arena, vec4, image.width * image.height);

            int32 index = 0;
            For (y, image.height) {
                
                real32 yT = y / (image.height * 1.0f);

                For (x, image.width) {
                    real32 xT = x / (image.width * 1.0f);

                    if (useHSV) {
                        vec3 hsv = V3(Lerp(minHSV.x, maxHSV.x, xT),
                                      Lerp(minHSV.y, maxHSV.y, yT),
                                      Lerp(minHSV.z, maxHSV.z, zT));

                        image.colors[index++] = V4(HSVToRGB(hsv), 1.0f);
                    }
                    else {
                        image.colors[index++] =
                            V4(Lerp(minColor.x, maxColor.x, xT),
                               Lerp(minColor.y, maxColor.y, yT),
                               Lerp(minColor.z, maxColor.z, zT),
                               1.0f);
                    }
                }
            }
        }
    }

    For (i, canvas.cells.capacity) {
        PushBack(&canvas.cells, {});
    }

    Crawler &crawler = game.crawler;
    crawler.positionStack = MakeDynamicArray<vec2i>(&arena, canvas.cells.count);

    crawler.position =
        V2i(RandfRange(0, Mosaic->gridWidth), RandfRange(0, Mosaic->gridHeight));

    game.crawler.colorPosition.x = RandfRange(0, canvas.colorCube.size);
    game.crawler.colorPosition.y = RandfRange(0, canvas.colorCube.size);
    game.crawler.colorPosition.z = RandfRange(0, canvas.colorCube.size);
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

#if 1
    {
        DynamicArray<int32> neighbors = MakeDynamicArray<int32>(&Game->frameMem, 16);

        vec3i maxCoord = V3i(canvas.colorCube.size - 1,
                             canvas.colorCube.size - 1,
                             canvas.colorCube.size - 1);

        for (int z = -1; z <= 1; z++) {
            for (int y = -1; y <= 1; y++) {
                for (int x = -1; x <= 1; x++) {
                    vec3i newPosition =
                        Clamp(crawler.colorPosition + V3i(x, y, z),
                              V3i(0, 0, 0), maxCoord);
                    int32 index =
                        GetColorIndex(newPosition, canvas.colorCube);
                    if (index >= 0) {
                        PushBack(&neighbors, index);
                    }
                }
            }
        }

        int32 newIndex = neighbors[(int32)RandfRange(0, neighbors.count)];
        crawler.colorPosition = GetColorPosition(newIndex, canvas.colorCube);
    }
#endif

    int32 newIndex = GetCellIndex(crawler.position, canvas);
    if (!canvas.cells[newIndex].filled) {
        canvas.filledCount++;
        canvas.cells[newIndex].filled = true;

        //canvas.cells[newIndex].color = V4(1, 1, 1, 1);
        // Image &image = canvas.colorCube.images[RandfRange(0, canvas.colorCube.size)];
        // canvas.cells[newIndex].color = image.colors[(int32)RandfRange(0, image.capacity)];

        Image &image = canvas.colorCube.images[crawler.colorPosition.z];
        canvas.cells[newIndex].color =
            image.colors[crawler.colorPosition.x + (crawler.colorPosition.y * image.width)];

        // // @HACK @UNDO
        // canvas.cells[newIndex].color = V4(0.25f, 0.25f, 0.25f, 1.0f);
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
