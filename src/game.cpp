
// The way includes work: are they just copy in the contents of the .h or .cpp file
// into 

#include "game.h"
#include "input.cpp"

#include "render.cpp"

#include "mesh.cpp"
#include "entity.cpp"

#include "mosaic.cpp"

const uint32 screenWidth = 1600;
const uint32 screenHeight = 900;

void GameInit(GameMemory *gameMem) {
    Game = gameMem;

    Game->screenWidth = screenWidth;
    Game->screenHeight = screenHeight;

    AllocateFrameMem(1024 * 256);

    // @TODO: super weird and bad we allocate the queue in the game and not the platform because
    // that's where we know how many devices we have obviously
    gameMem->inputQueue = AllocateInputQueue(32, 2);

    uint32 bytesPerPixel = 4;
    gameMem->pitch = bytesPerPixel * screenWidth;
    gameMem->bitmapSize = screenWidth * screenHeight * bytesPerPixel;
    gameMem->bitmap = (uint8 *)malloc(gameMem->bitmapSize * sizeof(uint8));
    memset(gameMem->bitmap, 0, gameMem->bitmapSize);

    // Font business
    {
        int32 fontBitmapWidth = 1024;
        int32 fontBitmapHeight = 1024;
        int32 fontBitmapSize = fontBitmapWidth * fontBitmapHeight;
        uint8 *fontBitmap = (uint8 *)malloc(fontBitmapSize);

        uint32 startAscii = 32;
        uint32 endAscii = 127;
        uint32 charCount = endAscii - startAscii;

        uint32 ttfBufferSize = 1024 * 10000;
        uint8 *ttfBuffer = (uint8 *)malloc(ttfBufferSize);
        FILE *ttfFile = fopen("data/DejaVuSansMono.ttf", "rb");
        fread(ttfBuffer, 1, ttfBufferSize, ttfFile);

        // @GACK @HARDCODED
        float fontPixelHeight = 40;
    
        stbtt_bakedchar *bakedChars = (stbtt_bakedchar *)malloc(sizeof(stbtt_bakedchar) * charCount);
    
        stbtt_BakeFontBitmap(ttfBuffer, 0, fontPixelHeight, fontBitmap, fontBitmapWidth, fontBitmapHeight, startAscii, charCount, bakedChars);

        Glyph *glyphs = (Glyph *)malloc(sizeof(Glyph) * charCount);

        for (int i = 0; i < charCount; i++) {
            glyphs[i].x0 = bakedChars[i].x0;
            glyphs[i].x1 = bakedChars[i].x1;

            glyphs[i].y0 = bakedChars[i].y0;
            glyphs[i].y1 = bakedChars[i].y1;

            glyphs[i].xOffset = bakedChars[i].xoff;
            glyphs[i].yOffset = bakedChars[i].yoff;
            glyphs[i].xAdvance = bakedChars[i].xadvance;
        }

        gameMem->glyphs = glyphs;
    
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
        }

        gameMem->fontSprite = fontSprite;
    }


    Camera *cam = &gameMem->camera;
    cam->type = CameraType_Orthographic;
    cam->width = 16;
    cam->height = 9;
    cam->projection = Orthographic(cam->width * -0.5f, cam->width * 0.5f,
                                   cam->height * -0.5f, cam->height * 0.5f,
                                   0.0, 100.0f);

    //cam->projection = Perspective(DegToRad(90.0f), 16 / 9.0f, 0.1f, 100.0f);

    gameMem->camAngle = 0;
    gameMem->cameraPosition = V3(0, 0, 3);
    gameMem->cameraRotation = AxisAngle(V3(0, 1, 0), gameMem->camAngle);
    //gameMem->cameraRotation = AxisAngle(V3(0, 1, 0), 0);
    
    //cam->view = LookAt(V3(3, 3, 3), V3(0), V3(0, 1, 0));

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
#endif

    

    
    LoadSprite(&gameMem->circleTexture, "data/circle.png");
    OpenGL_InitTexture(&gameMem->circleTexture);

    LoadSprite(&gameMem->mouseCursorTexture, "data/mouse_cursor.png");
    OpenGL_InitTexture(&gameMem->mouseCursorTexture);

    LoadSprite(&gameMem->galagaShip, "data/galaga_ship.png");
    OpenGL_InitTexture(&gameMem->galagaShip);

    LoadSprite(&gameMem->pixelTest, "data/pixel_test.png");
    OpenGL_InitTexture(&gameMem->pixelTest);


    MosaicInit(gameMem);
}

void GameUpdateAndRender(GameMemory *gameMem) {
    
    UpdateInput(&Game->inputQueue);

    InputQueue *input = &gameMem->inputQueue;

    if (InputPressed(input, Input_Escape)) {
        gameMem->running = false;
    }

    MosaicUpdate(gameMem);

    DeleteEntities(&Game->entityDB);

    gameMem->frame++;
    ClearFrameMem();
}
