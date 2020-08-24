
#define GAME_SERVER 0

#include "game.h"
#include "input.cpp"

#include "log.cpp"

#include "render.cpp"
#include "audio.cpp"

#include "network.cpp"

#include "mesh.cpp"
#include "entity.cpp"


#include "ui.cpp"

#include "game_code.cpp"

const uint32 screenWidth = 1600;
const uint32 screenHeight = 900;

void InitFont(FontTable *font, char *path) {
    int32 fontBitmapWidth = 1024;
    int32 fontBitmapHeight = 1024;
    int32 fontBitmapSize = fontBitmapWidth * fontBitmapHeight;
    uint8 *fontBitmap = (uint8 *)malloc(fontBitmapSize);

    // @HACK: actually figure this out.
    // It should be enough that when we grab a glyph and scale by emSize it should take up 1 unit.
    // This is totally arbitrarily chosen right now so that a glyph with size 1 is about 1 unit.
    font->emSize = 26;

    uint32 startAscii = 32;
    uint32 endAscii = 127;
    uint32 charCount = endAscii - startAscii;

    uint32 ttfBufferSize = 1024 * 10000;
    uint8 *ttfBuffer = (uint8 *)malloc(ttfBufferSize);
    FILE *ttfFile = fopen(path, "rb");
    //FILE *ttfFile = fopen("data/DejaVuSansMono.ttf", "rb");
    //FILE *ttfFile = fopen("data/LiberationSerif-Regular.ttf", "rb");
    
    //FILE *ttfFile = fopen("data/liberation-mono/LiberationMono-Regular.ttf", "rb");
    fread(ttfBuffer, 1, ttfBufferSize, ttfFile);

        
    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttfBuffer, 0);
    int32 ascent, descent;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, 0);

    font->ascent = ascent / (fontBitmapWidth * 1.0f);
    font->descent = descent / (fontBitmapWidth * 1.0f);

    // @GACK @HARDCODED: what is the relationship of this to the bitmap size?
    float fontPixelHeight = 64;
    
    stbtt_bakedchar *bakedChars = (stbtt_bakedchar *)malloc(sizeof(stbtt_bakedchar) * charCount);
    
    stbtt_BakeFontBitmap(ttfBuffer, 0, fontPixelHeight, fontBitmap, fontBitmapWidth, fontBitmapHeight, startAscii, charCount, bakedChars);

    font->glyphs = (Glyph *)malloc(sizeof(Glyph) * charCount);
    Glyph *glyphs = font->glyphs;
    
    font->texcoordsMapData = (vec4 *)malloc(sizeof(vec4) * charCount);

    font->glyphCount = charCount;

    for (int i = 0; i < charCount; i++) {
        glyphs[i].xOffset = bakedChars[i].xoff / (fontBitmapWidth * 1.0f);
        glyphs[i].yOffset = bakedChars[i].yoff / (fontBitmapWidth * 1.0f);
        glyphs[i].xAdvance = bakedChars[i].xadvance / (fontBitmapWidth * 1.0f);

        stbtt_aligned_quad quad;

        real32 x = 0;
        real32 y = 0;
        stbtt_GetBakedQuad(bakedChars, fontBitmapWidth, fontBitmapHeight, i, &x, &y, &quad, 1);

        //Print("%c (%f %f) (%f %f) adv %f", i + startAscii, quad.x0, quad.y0, quad.x1, quad.y1, bakedChars[i].xadvance);

        // This calculation depends on our quad being 1 wide and 1 tall, 2x2 breaks things
        // Not exactly sure why... 
        glyphs[i].lowerLeft = V2(quad.x0 / (fontBitmapWidth * 1.0f), -quad.y1 / (fontBitmapHeight * 1.0f));
#if 1
        font->texcoordsMapData[i] = V4(bakedChars[i].x0 / (fontBitmapWidth * 1.0f),
                                       bakedChars[i].y0 / (fontBitmapWidth * 1.0f),
                                       bakedChars[i].x1 / (fontBitmapWidth * 1.0f),
                                       bakedChars[i].y1 / (fontBitmapWidth * 1.0f));
#endif
    }
    
    Sprite fontSprite;
    fontSprite.width = fontBitmapWidth;
    fontSprite.height = fontBitmapHeight;
    fontSprite.size = fontSprite.width * fontSprite.height * 4;
    fontSprite.data = (uint8 *)malloc(fontSprite.size);

    for (int i = 0; i < fontBitmapSize; i++) {
        fontSprite.data[(i * 4) + 0] = fontBitmap[i];
        fontSprite.data[(i * 4) + 1] = fontBitmap[i];
        fontSprite.data[(i * 4) + 2] = fontBitmap[i];
        fontSprite.data[(i * 4) + 3] = fontBitmap[i];

#if 0
        // @BUG: this produces a bunch of dots
        if (i < 1024 * 4) {
            fontSprite.data[(i * 4) + 0] = 255;
            fontSprite.data[(i * 4) + 1] = 0;
            fontSprite.data[(i * 4) + 2] = 0;
            fontSprite.data[(i * 4) + 3] = 0;
        }
        else {
            fontSprite.data[(i * 4) + 0] = 0; // 255; // 255 * ((1 + sinf(i * 0.1f)) / 2.0f); 
            fontSprite.data[(i * 4) + 1] = 0;
            fontSprite.data[(i * 4) + 2] = 0;
            fontSprite.data[(i * 4) + 3] = 0;
        }
#endif
            
    }

    font->texture = fontSprite;

    OpenGL_InitFontTable(font);
}

void GameInit(GameMemory *gameMem) {
    Game = gameMem;
    Input = &Game->inputQueue;

    Game->screenWidth = screenWidth;
    Game->screenHeight = screenHeight;

    AllocateMemoryArena(&Game->frameMem, Megabytes(1024));

    Game->log.head = (DebugLogNode *)malloc(sizeof(DebugLogNode));
    AllocateDebugLogNode(Game->log.head, LOG_BUFFER_CAPACITY);
    Game->log.current = Game->log.head;
    Game->log.head->next = NULL;

    // @TODO: super weird and bad we allocate the queue in the game and not the platform because
    // that's where we know how many devices we have obviously
    gameMem->inputQueue = AllocateInputQueue(32, 2);

    Camera *cam = &gameMem->camera;
    cam->type = CameraType_Orthographic;
    cam->width = 16;
    cam->height = 9;
    cam->projection = Orthographic(cam->width * -0.5f, cam->width * 0.5f,
                                   cam->height * -0.5f, cam->height * 0.5f,
                                   0.0, 100.0f);

    gameMem->camAngle = 0;
    gameMem->cameraPosition = V3(0, 0, 3);
    gameMem->cameraRotation = AxisAngle(V3(0, 1, 0), gameMem->camAngle);

    mat4 camWorld = TRS(gameMem->cameraPosition, gameMem->cameraRotation, V3(1));
    cam->view = OrthogonalInverse(camWorld);
    
    cam->viewProjection = cam->projection * cam->view;

    
    // INIT GRAPHICS
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    AllocateTriangle(&gameMem->tri);
    OpenGL_InitMesh(&gameMem->tri);

    AllocateQuad(&gameMem->quad);
    OpenGL_InitMesh(&gameMem->quad);

    AllocateGlyphQuad(&gameMem->glyphQuad);
    OpenGL_InitMesh(&gameMem->glyphQuad);

    AllocateQuadTopLeft(&gameMem->quadTopLeft);
    OpenGL_InitMesh(&gameMem->quadTopLeft);

    InitFont(&gameMem->monoFont, "data/DejaVuSansMono.ttf");
    InitFont(&gameMem->serifFont, "data/LiberationSerif-Regular.ttf");
    // Setup glyph buffers
    {
        for (int i = 0; i < 32; i++) {
            GlyphBuffer *buffer = &Game->glyphBuffers[i];
            
            buffer->capacity = GlyphBufferCapacity;
            buffer->size = buffer->capacity * sizeof(GlyphData);
            buffer->data = (GlyphData *)malloc(buffer->size);
            memset(buffer->data, 0, buffer->size);

            glGenBuffers(1, (GLuint *)&buffer->bufferID);
            glBindBuffer(GL_ARRAY_BUFFER, buffer->bufferID);
            glBufferData(GL_ARRAY_BUFFER, buffer->size, buffer->data, GL_STATIC_DRAW);
        }
    }


#if WINDOWS
    {
        LoadShader("shaders/mesh.vert", "shaders/mesh.frag", &gameMem->shader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "color",
        };
        CompileShader(&gameMem->shader, 3, uniforms);
    }

    {
        LoadShader("shaders/instanced_quad_shader.vert", "shaders/instanced_quad_shader.frag", &gameMem->instancedQuadShader);
        const char *uniforms[] = {
            "viewProjection",
        };
        CompileShader(&gameMem->instancedQuadShader, 1, uniforms);
    }

    {
        LoadShader("shaders/textured_quad.vert", "shaders/textured_quad.frag", &gameMem->texturedQuadShader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "texture0",
        };
        CompileShader(&gameMem->texturedQuadShader, 3, uniforms);
    }

        {
        LoadShader("shaders/text.vert", "shaders/text.frag", &gameMem->textShader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "texcoordsMap",
            "fontTable",
            "time",
        };
        CompileShader(&gameMem->textShader, ARRAY_LENGTH(char *, uniforms), uniforms);
    }
        
#elif LINUX
    {
        LoadShader("shaders/mesh_pi.vert", "shaders/mesh_pi.frag", &gameMem->shader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "color",
        };
        CompileShader(&gameMem->shader, 3, uniforms);
    }

    // {
    //     LoadShader("shaders/instanced_quad_shader.vert", "shaders/instanced_quad_shader.frag", &gameMem->instancedQuadShader);
    //     const char *uniforms[] = {
    //         "viewProjection",
    //     };
    //     CompileShader(&gameMem->instancedQuadShader, 1, uniforms);
    // }

    // {
    //     LoadShader("shaders/textured_quad.vert", "shaders/textured_quad.frag", &gameMem->texturedQuadShader);
    //     const char *uniforms[] = {
    //         "model",
    //         "viewProjection",
    //         "texture0",
    //     };
    //     CompileShader(&gameMem->texturedQuadShader, 3, uniforms);
    // }
#endif

    MyInit();
}

void GameDeinit() {
    WriteLogToFile("output/log.txt");
}


void WriteSoundSamples(GameMemory *game, int32 sampleCount, real32 *buffer) {
    PlayAudio(&game->audioPlayer, sampleCount, buffer);
}

void GameUpdateAndRender(GameMemory *gameMem) {
    
    UpdateInput(&Game->inputQueue);

    InputQueue *input = &gameMem->inputQueue;

    if (InputPressed(input, Input_Escape)) {
        gameMem->running = false;
    }

    Game->currentGlyphBufferIndex = 0;

    // Want to get them all before we decide what to do in our frame.
    ReceivePackets();

    // @TODO: pick a key to step frame and then check if that's pressed
    // We want to do this before the update obviously

    if (!Game->paused || Game->steppingFrame) {
        MyGameUpdate();
    }

    Game->steppingFrame = false;

    RenderRectBuffer(&Game->rectBuffer);
    Game->rectBuffer.count = 0;
    
    DrawGlyphs(gameMem->glyphBuffers);
    
    DeleteEntities(&Game->entityDB);

    // Send all the packets we've accumulated. 
    SendPackets();

    Game->fps = (real32)Game->frame / (Game->time - Game->startTime);

    gameMem->frame++;
    ClearMemoryArena(&Game->frameMem);

    ClearInputQueue(input);
}
