
#define GAME_SERVER 0

#include "game.h"
#include "input.cpp"

#include "file_io.cpp"

#include "log.cpp"

#include "render.cpp"
#include "audio.cpp"
#include "synth.cpp"

#include "network.cpp"

#include "mesh.cpp"
#include "font.cpp"

#include "ui.cpp"

#include "game_code.cpp"


bool ReadConfigFile(char *path) {
    enum ConfigTokenType {
        ConfigTokenType_Identifier,
        ConfigTokenType_Colon,
        ConfigTokenType_Int,
        ConfigTokenType_Float,
        ConfigTokenType_String,
    };

    struct ConfigToken : Token {
        ConfigTokenType configType;
    };

    FileHandle file = {};
    if (!OpenFileForRead(path, &file)) {
        return false;
    }

    DynamicArray<ConfigToken> tokens = MakeDynamicArray<ConfigToken>(&Core->frameMem, 32);

    while (file.offset < file.size) {
        ConsumeBytesPassing(&file, IsWhitespace);

        if (file.offset >= file.size) break;

        ConfigToken t = {};
        t.start = (char *)(&file.data[file.offset]);

        if (ConsumeIdentifierToken(&file, &t.start, &t.length)) {
            t.configType = ConfigTokenType_Identifier;
        }
        else if (ConsumeByteMatching(&file, ':')) {
            t.length = 1;
            t.configType = ConfigTokenType_Colon;
        }
        else if (ConsumeFloatLiteral(&file, &t.start, &t.length)) {
            t.configType = ConfigTokenType_Float;
        }
        else if (ConsumeIntLiteral(&file, &t.start, &t.length)) {
            t.configType = ConfigTokenType_Int;
        }
        else if (ConsumeStringLiteral(&file, &t.start, &t.length)) {
            t.configType = ConfigTokenType_String;
        }
        else {
            Print("Config tokenizer error: unexpected character '%c'", file.data[file.offset]);
            ASSERT(false);
        }

        PushBack(&tokens, t);
    }

    int32 tokenIndex = 0;
    while (tokenIndex < tokens.count) {
        ConfigToken t = tokens[tokenIndex];

        if (t.configType != ConfigTokenType_Identifier) {
            tokenIndex++;
            continue;
        }

        if (StringEquals(t.start, "resolutionWidth", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Core->graphics.resolutionWidth = atoi(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "resolutionHeight", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Core->graphics.resolutionHeight = atoi(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "windowWidth", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Core->graphics.windowWidth = atoi(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "windowHeight", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Core->graphics.windowHeight = atoi(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "fullscreen", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Core->graphics.fullscreen = atoi(tokens[tokenIndex].start) != 0;
                }
            }
        }
        else if (StringEquals(t.start, "volume", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Float) {
                    Core->audioPlayer.volume = atof(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "server_ip", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_String) {
                    Core->networkInfo.serverIPString = (char *)malloc(tokens[tokenIndex].length + 1);
                    memcpy(Core->networkInfo.serverIPString, tokens[tokenIndex].start, tokens[tokenIndex].length);
                    Core->networkInfo.serverIPString[tokens[tokenIndex].length] = '\0';
                }
            }
        }
        else if (StringEquals(t.start, "socket_port", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Core->networkInfo.configPort = atoi(tokens[tokenIndex].start);
                }
            }
        }

        tokenIndex++;
    }

    CloseFile(&file);
    return true;
}

void GameInit(CoreMemory *coreMem) {
    Core = coreMem;
    Input = &Core->inputManager;
    UI = &Core->graphics.uiManager;

    Core->log.head = (DebugLogNode *)malloc(sizeof(DebugLogNode));
    AllocateDebugLogNode(Core->log.head, LOG_BUFFER_CAPACITY);
    Core->log.current = Core->log.head;
    Core->log.head->next = NULL;
    Core->log.logs = MakeDynamicArray<LogData>(&Core->permanentArena, 1024);

    Camera *cam = &coreMem->camera;
    cam->size = 1;
    cam->type = CameraType_Orthographic;
    cam->width = 16;
    cam->height = 9;
    cam->projection = Orthographic(cam->width * -0.5f * cam->size, cam->width * 0.5f * cam->size,
                                   cam->height * -0.5f * cam->size, cam->height * 0.5f * cam->size,
                                   0.0, 100.0f);

    cam->position = V3(0, 0, 3);
    cam->rotation = AxisAngle(V3(0, 1, 0), 0.0f);

    UpdateCamera(cam);
    
    // INIT GRAPHICS
    AllocateTriangle(&coreMem->graphics.tri);
    InitMesh(&coreMem->graphics.tri);

    AllocateQuad(&coreMem->graphics.quad);
    InitMesh(&coreMem->graphics.quad);

    AllocateGlyphQuad(&coreMem->graphics.glyphQuad);
    InitMesh(&coreMem->graphics.glyphQuad);

    AllocateQuadTopLeft(&coreMem->graphics.quadTopLeft);
    InitMesh(&coreMem->graphics.quadTopLeft);

    AllocateCube(&coreMem->graphics.cube);
    InitMesh(&coreMem->graphics.cube);

    InitFont(&coreMem->graphics.monoFont, "data/DejaVuSansMono.ttf");
    InitFont(&coreMem->graphics.serifFont, "data/LiberationSerif-Regular.ttf");

    InitGlyphBuffers(GlyphBufferCount);

#if WINDOWS || LINUX
    {
        LoadShader("shaders/mesh.vert", "shaders/mesh.frag", &coreMem->graphics.shader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "color",
        };
        CompileShader(&coreMem->graphics.shader, 3, uniforms);
    }

#if 0
    {
        LoadShader("shaders/cool_mesh.vert", "shaders/cool_mesh.frag", &coreMem->graphics.coolShader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "color",
            "time",
        };
        CompileShader(&coreMem->graphics.coolShader, 4, uniforms);
    }
#endif

    {
        LoadShader("shaders/instanced_quad_shader.vert", "shaders/instanced_quad_shader.frag", &coreMem->graphics.instancedQuadShader);
        const char *uniforms[] = {
            "viewProjection",
        };
        CompileShader(&coreMem->graphics.instancedQuadShader, 1, uniforms);
    }

    {
        LoadShader("shaders/instanced_texture_quad_shader.vert", "shaders/instanced_texture_quad_shader.frag", &coreMem->graphics.instancedTextureQuadShader);
        const char *uniforms[] = {
            "viewProjection",
            "texture0",
        };
        CompileShader(&coreMem->graphics.instancedTextureQuadShader, ARRAY_LENGTH(char *, uniforms), uniforms);
    }

    {
        LoadShader("shaders/textured_quad.vert", "shaders/textured_quad.frag", &coreMem->graphics.texturedQuadShader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "texture0",
        };
        CompileShader(&coreMem->graphics.texturedQuadShader, ARRAY_LENGTH(char *, uniforms), uniforms);
    }

    {
        LoadShader("shaders/text.vert", "shaders/text.frag", &coreMem->graphics.textShader);
        const char *uniforms[] = {
                                   "model",
                                   "viewProjection",
                                   "texcoordsMap",
                                   "fontTable",
        };
        CompileShader(&coreMem->graphics.textShader, ARRAY_LENGTH(char *, uniforms), uniforms);
    }
#endif

    AudioPlayerInit(&Core->audioPlayer, &Core->permanentArena);

    AllocateRectBuffer(256 * 256, &Core->graphics.rectBuffer);
    AllocateSpriteBuffer(256 * 256, &Core->graphics.spriteBuffer);

    Core->graphics.uiCommands = MakeDynamicArray<UICommand>(&Core->permanentArena, 64);
    SetupUIRenderTarget(&Core->graphics.uiTarget, coreMem->graphics.resolutionWidth, coreMem->graphics.resolutionHeight);
    SetupUIRenderTarget(&Core->graphics.frameTarget, coreMem->graphics.resolutionWidth, coreMem->graphics.resolutionHeight);

#if WINDOWS || LINUX
    {
        LoadShader("shaders/blit.vert", "shaders/blit.frag", &coreMem->graphics.blitShader);
        const char *blitUniforms[] = { "model", "viewProjection", "uiTexture" };
        CompileShader(&coreMem->graphics.blitShader, 3, blitUniforms);
    }
#endif

    MyGameInit();
}

void GameDeinit() {
    if (IS_SERVER) {
        WriteLogToFile("output/server_log.txt");    
    }
    else {
        WriteLogToFile("output/log.txt");    
    }
}


void WriteSoundSamples(CoreMemory *core, int32 sampleCount, real32 *buffer) {
    PlayAudio(&Core->audioPlayer, sampleCount, buffer);
}

void GameUpdateAndRender(CoreMemory *core) {
    
    UpdateInput(&Core->inputManager);

    InputManager *input = &Core->inputManager;

    if (InputPressed(Core->keyboard, Input_Escape) && UI->activeID == 0) {
        Core->running = false;
    }

    Core->graphics.currentGlyphBufferIndex = 0;

    // @TODO: pick a key to step frame and then check if that's pressed
    // We want to do this before the update obviously

    if (!Core->paused || Core->steppingFrame) {
        MyGameUpdate();
    }

    Camera *cam = &Core->camera;
    UpdateCamera(&Core->camera);

    {
      // this is assuming 2D
      vec2 mousePosRelativeToCamera =
        V2(Input->mousePosNormSigned.x * ((cam->size * cam->width) / 2),
           Input->mousePosNormSigned.y * ((cam->size * cam->height) / 2));

      Ray ray =
        MakeRay(cam->position + V3(mousePosRelativeToCamera, 0),
                V3(0, 0, -1));

      float32 t;
      RaycastPlane(V3(0, 0, 0), V3(0, 0, 1), ray, &t);
        
      Input->mousePosWorld = PointAt(ray, t).xy;
    }


    Core->steppingFrame = false;

    RenderRectBuffer(&Core->graphics.rectBuffer);
    Core->graphics.rectBuffer.count = 0;

    RenderSpriteBuffer(&Core->graphics.spriteBuffer);
    Core->graphics.spriteBuffer.count = 0;
    
    DrawGlyphs(Core->graphics.glyphBuffers);
    FlushUICommands();
    
    //DeleteEntities(&Core->graphics.entityDB);
    
    Core->fps = (real32)Core->frame / (Core->time - Core->startTime);

    Core->frame++;
    ClearMemoryArena(&Core->frameMem);

    ClearInputManager(input);
}
