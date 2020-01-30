
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "types.h"
#include "commonlib.h"

#include "math/math.h"
#include "memory.h"

//#include "dynamic_array.h"

#include "render.h"
#include "input.h"
#include "collections.h"

#include "camera.h"
#include "mesh.h"
#include "entity.h"

#include "mosaic.h"


#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define HERTZ 60.0f

#define FRAME_RATE 1 / HERTZ

struct GameMemory {
    bool running;

    real32 systemTime;
    real32 time;
    real32 deltaTime;

    uint32 frame;
    
    uint32 screenWidth;
    uint32 screenHeight;
    uint32 pitch;

    uint32 bitmapSize;
    uint8 *bitmap;

    Glyph *glyphs;
    Sprite fontSprite;

    Camera camera;
    vec3 cameraPosition;
    quaternion cameraRotation;
    real32 camAngle;

    Shader texturedQuadShader;

    Sprite circleTexture;
    Sprite mouseCursorTexture;
    Sprite galagaShip;
    Sprite pixelTest;
    
    Shader shader;
    int32 vertBuffer;

    Mesh tri;
    Mesh quad;

    InputQueue inputQueue;

    EntityDB entityDB;

    int32 gameIndex;

    MosaicMem mosaic;
};

GameMemory *Game = NULL;

#define ArrayLength(array, type) sizeof(array)/sizeof(type)
