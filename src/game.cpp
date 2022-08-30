
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
    FILE *file = fopen(path, "r");

    if (file != NULL) {
        int c = fgetc(file);

        enum ConfigState {
            ConfigState_Invalid,
            ConfigState_ScreenWidth,
            ConfigState_ScreenHeight,
            ConfigState_Volume,
            ConfigState_ServerIP,
            ConfigState_Port,
        };

        ConfigState state = ConfigState_ScreenWidth;

        char currentToken[64];
        memset(currentToken, 0, 64);
        
        int32 tokenLength = 0;
        bool parsedToken = false;

        // @NOTE: this is not an elegant way to do this
        // It would be much nicer if we broke it into tokens first.
        // It would also be nice if we had more file reading features
        while (c != EOF) {
            if (c == '\n' || c == ' ') {
                goto nextChar;
            }
                
            if (state == ConfigState_ScreenWidth) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }

                if (!parsedToken) {
                    if (strcmp(currentToken, "screenWidth:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->screenWidth = atoi(currentToken);
                        state = ConfigState_ScreenHeight;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
            }

            if (state == ConfigState_ScreenHeight) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }
                
                if (!parsedToken) {
                    if (strcmp(currentToken, "screenHeight:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->screenHeight = atoi(currentToken);
                        state = ConfigState_Volume;

                        state = ConfigState_Volume;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
                
            }

            if (state == ConfigState_Volume) {

                if (c != ';') {
                currentToken[tokenLength++] = c;
                }
                
                if (!parsedToken) {
                    if (strcmp(currentToken, "volume:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                    Game->audioPlayer.volume = atof(currentToken);

                    state = ConfigState_ServerIP;
                    tokenLength = 0;
                    memset(currentToken, 0, 64);
                    parsedToken = false;
                    }
                }
            }

            if (state == ConfigState_ServerIP) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }
                
                if (!parsedToken) {
                    if (strcmp(currentToken, "server_ip:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->networkInfo.serverIPString = (char *)malloc(tokenLength + 1);
                        memcpy(Game->networkInfo.serverIPString, currentToken, tokenLength + 1);

                        state = ConfigState_Port;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
            }

            if (state == ConfigState_Port) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }
                
                if (!parsedToken) {
                    if (strcmp(currentToken, "socket_port:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->networkInfo.configPort = atoi(currentToken);

                        state = ConfigState_Invalid;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
            }

        nextChar:
            c = fgetc(file);
        }

        return true;
    }
    else {
        return false;
    }
}

void GameInit(GameMemory *gameMem) {
    Game = gameMem;
    Input = &Game->inputManager;

    AllocateMemoryArena(&Game->permanentArena, Megabytes(256));
    AllocateMemoryArena(&Game->frameMem, Megabytes(32));

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

    gameMem->camAngle = 0;
    gameMem->cameraPosition = V3(0, 0, 3);
    gameMem->cameraRotation = AxisAngle(V3(0, 1, 0), gameMem->camAngle);

    UpdateCamera(cam, gameMem->cameraPosition, gameMem->cameraRotation);

    
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
            "time",
        };
        CompileShader(&gameMem->texturedQuadShader, 4, uniforms);
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
#endif

    AudioPlayerInit(&Game->audioPlayer, &Game->permanentArena);

    AllocateRectBuffer(256 * 256, &Game->rectBuffer);

    MyInit();
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
    UpdateCamera(&gameMem->camera, gameMem->cameraPosition, gameMem->cameraRotation);

    Game->steppingFrame = false;

    RenderRectBuffer(&Game->rectBuffer);
    Game->rectBuffer.count = 0;
    
    DrawGlyphs(gameMem->glyphBuffers);
    
    //DeleteEntities(&Game->entityDB);
    
    Game->fps = (real32)Game->frame / (Game->time - Game->startTime);

    gameMem->frame++;
    ClearMemoryArena(&Game->frameMem);

    ClearInputManager(input);
}
