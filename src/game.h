
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "types.h"

#include "commonlib.h"
#include "strings.h"

#include "my_memory.h"

#include "hashtable.h"
#include "dynamic_array.h"


#include "log.h"
#include "network.h"

#include "math/math.h"


#include "audio.h"
#include "synth.h"
#include "input.h"
#include "collections.h"

#include "camera.h"
#include "mesh.h"
#include "ui.h"

#include "render.h"


// Forward declaration to avoid circular dependency
struct CoreGraphics;

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
#define UIClipStackMax 32
#define UIGlyphCmdCapacity 512

struct CoreMemory {
    bool running;
    bool paused;
    bool steppingFrame;

    real32 systemTime;
    real32 time;
    real32 deltaTime;
    real32 startTime;

    uint32 frame;
    real32 fps;

    MemoryArena permanentArena;

    DebugLog log;
    MemoryArena frameMem;

    NetworkInfo networkInfo;

    Camera camera;

    AudioPlayer audioPlayer;

    InputManager inputManager;
    InputDevice *keyboard;
    InputDevice *mouse;

    int32 gameIndex;

    bool inputStringActive;
    char inputString[255];

    void *myData;

    CoreGraphics graphics;

    // @TODO: shaders, meshes, etc.
    GlyphBuffer *glyphBuffers;
    Shader *texturedQuadShader;
    Mesh *quad;
    vec3 quadTopLeft;
    Shader *shader;
    Shader *coolShader;
    Shader *instancedQuadShader;
};

real32 Time = 0;
real32 DeltaTime = 0;

MAllocator Heap = { DefaultAllocatorAllocate, DefaultAllocatorDeallocate };

CoreMemory *Core = NULL;
InputManager *Input = NULL;

InputDevice *Keyboard = NULL;
InputDevice *Mouse = NULL;

UIManager *UI = NULL;

#define ArrayLength(array, type) sizeof(array)/sizeof(type)

