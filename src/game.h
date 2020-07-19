
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "types.h"

#include "commonlib.h"

#include "dynamic_array.h"


#include "network.h"

#include "math/math.h"
#include "my_memory.h"

#include "render.h"
#include "audio.h"
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

struct FontTable {
    int32 glyphCount;
    Sprite texture;

    vec4 *texcoordsMapData;
    
    int32 texcoordsMapID;

    real32 ascent;
    real32 descent;
};

// @NOTE: there are only 32 buffers for text rendering so on one frame you can only have
//        32 calls to DrawText(). You can increase this number if you're willing to allocate
//        more memory.
#define GlyphBufferCount 32
#define GlyphBufferCapacity 64

struct GameMemory {
    bool running;

    real32 systemTime;
    real32 time;
    real32 deltaTime;

    real32 startTime;

    uint32 frame;
    real32 fps;
    
    uint32 screenWidth;
    uint32 screenHeight;
    uint32 pitch;

    MemoryArena frameMem;

    NetworkInfo networkInfo;

    Glyph *glyphs;
    int32 currentGlyphBufferIndex;
    GlyphBuffer glyphBuffers[GlyphBufferCount];
    FontTable font;

    Camera camera;
    vec3 cameraPosition;
    quaternion cameraRotation;
    real32 camAngle;

    AudioPlayer audioPlayer;

    Shader texturedQuadShader;

    Shader instancedQuadShader;
    
    Shader shader;
    int32 vertBuffer;

    Shader textShader;

    Mesh tri;
    Mesh quad;
    Mesh glyphQuad;
    Mesh quadTopLeft;

    InputQueue inputQueue;

    EntityDB entityDB;

    int32 gameIndex;

    MosaicMem mosaic;

    bool inputStringActive;
    char inputString[255];

    RectBuffer rectBuffer;
};

GameMemory *Game = NULL;
InputQueue *Input = NULL;

#define ArrayLength(array, type) sizeof(array)/sizeof(type)
