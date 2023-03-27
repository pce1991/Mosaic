
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "types.h"

#include "commonlib.h"

#include "my_memory.h"

#include "hashtable.h"
#include "dynamic_array.h"


#include "log.h"
#include "network.h"

#include "math/math.h"


#include "render.h"
#include "audio.h"
#include "input.h"
#include "collections.h"

#include "camera.h"
#include "mesh.h"

#include "mosaic.h"


#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define HERTZ 60.0f

#define FRAME_RATE 1 / HERTZ

// @NOTE: there are only 32 buffers for text rendering so on one frame you can only have
//        32 calls to DrawText(). You can increase this number if you're willing to allocate
//        more memory.
#define GlyphBufferCount 256
#define GlyphBufferCapacity 256

struct GameMemory {
    bool running;
    bool paused;
    bool steppingFrame;

    real32 systemTime;
    real32 time;
    real32 deltaTime;
    real32 startTime;

    uint32 frame;
    real32 fps;
    
    uint32 screenWidth;
    uint32 screenHeight;
    uint32 pitch;

    // This is memory that we allocate up front and will last for the duration of the
    // game.
    MemoryArena permanentArena;

    DebugLog log;
    MemoryArena frameMem;

    NetworkInfo networkInfo;

    int32 currentGlyphBufferIndex;
    GlyphBuffer glyphBuffers[GlyphBufferCount];

    FontTable monoFont;
    FontTable serifFont;

    Camera camera;
    vec3 cameraPosition;
    quaternion cameraRotation;
    real32 camAngle;

    AudioPlayer audioPlayer;

    Shader texturedQuadShader;

    Shader instancedQuadShader;
    
    Shader shader;

    Shader singleLight;

    Shader coolShader;

    Shader textShader;

    Mesh tri;
    Mesh quad;
    Mesh glyphQuad;
    Mesh quadTopLeft;
    Mesh cube;

    InputManager inputManager;
    InputDevice *keyboard;
    InputDevice *mouse;

    int32 gameIndex;

    bool inputStringActive;
    char inputString[255];

    RectBuffer rectBuffer;

    void *myData;
};

real32 Time = 0;
real32 DeltaTime = 0;

GameMemory *Game = NULL;
InputManager *Input = NULL;

InputDevice *Keyboard = NULL;
InputDevice *Mouse = NULL;

#define ArrayLength(array, type) sizeof(array)/sizeof(type)

