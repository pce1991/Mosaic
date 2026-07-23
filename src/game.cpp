
#define GAME_SERVER 0

#include "game.h"
#include "input.cpp"

#include "file_io.cpp"

#include "log.cpp"

#include "render.cpp"
#include "audio.cpp"

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

    DynamicArray<ConfigToken> tokens = MakeDynamicArray<ConfigToken>(&Game->frameMem, 32);

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

        if (StringEquals(t.start, "screenWidth", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Game->screenWidth = atoi(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "screenHeight", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Game->screenHeight = atoi(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "volume", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Float) {
                    Game->audioPlayer.volume = atof(tokens[tokenIndex].start);
                }
            }
        }
        else if (StringEquals(t.start, "server_ip", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_String) {
                    Game->networkInfo.serverIPString = (char *)malloc(tokens[tokenIndex].length + 1);
                    memcpy(Game->networkInfo.serverIPString, tokens[tokenIndex].start, tokens[tokenIndex].length);
                    Game->networkInfo.serverIPString[tokens[tokenIndex].length] = '\0';
                }
            }
        }
        else if (StringEquals(t.start, "socket_port", t.length)) {
            tokenIndex++;
            if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Colon) {
                tokenIndex++;
                if (tokenIndex < tokens.count && tokens[tokenIndex].configType == ConfigTokenType_Int) {
                    Game->networkInfo.configPort = atoi(tokens[tokenIndex].start);
                }
            }
        }

        tokenIndex++;
    }

    CloseFile(&file);
    return true;
}

void GameInit(GameMemory *gameMem) {
    Game = gameMem;
    Input = &Game->inputManager;
    UI = &Game->uiManager;

    Game->log.head = (DebugLogNode *)malloc(sizeof(DebugLogNode));
    AllocateDebugLogNode(Game->log.head, LOG_BUFFER_CAPACITY);
    Game->log.current = Game->log.head;
    Game->log.head->next = NULL;

    Camera *cam = &gameMem->camera;
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
    AllocateTriangle(&gameMem->tri);
    InitMesh(&gameMem->tri);

    AllocateQuad(&gameMem->quad);
    InitMesh(&gameMem->quad);

    AllocateGlyphQuad(&gameMem->glyphQuad);
    InitMesh(&gameMem->glyphQuad);

    AllocateQuadTopLeft(&gameMem->quadTopLeft);
    InitMesh(&gameMem->quadTopLeft);

    AllocateCube(&gameMem->cube);
    InitMesh(&gameMem->cube);

    InitFont(&gameMem->monoFont, "data/DejaVuSansMono.ttf");
    InitFont(&gameMem->serifFont, "data/LiberationSerif-Regular.ttf");

    InitGlyphBuffers(GlyphBufferCount);

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

#if 0
    {
        LoadShader("shaders/cool_mesh.vert", "shaders/cool_mesh.frag", &gameMem->coolShader);
        const char *uniforms[] = {
            "model",
            "viewProjection",
            "color",
            "time",
        };
        CompileShader(&gameMem->coolShader, 4, uniforms);
    }
#endif

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
        CompileShader(&gameMem->texturedQuadShader, ARRAY_LENGTH(char *, uniforms), uniforms);
    }

    {
        LoadShader("shaders/text.vert", "shaders/text.frag", &gameMem->textShader);
        const char *uniforms[] = {
                                  "model",
                                  "viewProjection",
                                  "texcoordsMap",
                                  "fontTable",
        };
        CompileShader(&gameMem->textShader, ARRAY_LENGTH(char *, uniforms), uniforms);
    }
#endif

    AudioPlayerInit(&Game->audioPlayer, &Game->permanentArena);

    AllocateRectBuffer(256 * 256, &Game->rectBuffer);

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


void WriteSoundSamples(GameMemory *game, int32 sampleCount, real32 *buffer) {
    PlayAudio(&game->audioPlayer, sampleCount, buffer);
}

void GameUpdateAndRender(GameMemory *gameMem) {
    
    UpdateInput(&Game->inputManager);

    InputManager *input = &gameMem->inputManager;

    if (InputPressed(Game->keyboard, Input_Escape)) {
        gameMem->running = false;
    }

    Game->currentGlyphBufferIndex = 0;

    // @TODO: pick a key to step frame and then check if that's pressed
    // We want to do this before the update obviously

    if (!Game->paused || Game->steppingFrame) {
        MyGameUpdate();
    }

    Camera *cam = &gameMem->camera;
    UpdateCamera(&gameMem->camera);

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


    Game->steppingFrame = false;

    RenderRectBuffer(&Game->rectBuffer);
    Game->rectBuffer.count = 0;
    
    DrawUIGlyphs();
    DrawGlyphs(gameMem->glyphBuffers);
    
    //DeleteEntities(&Game->entityDB);
    
    Game->fps = (real32)Game->frame / (Game->time - Game->startTime);

    gameMem->frame++;
    ClearMemoryArena(&Game->frameMem);

    ClearInputManager(input);
}
