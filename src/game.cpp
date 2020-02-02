
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
    Input = &Game->inputQueue;

    Game->screenWidth = screenWidth;
    Game->screenHeight = screenHeight;

    AllocateFrameMem(1024 * 256);

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

    MosaicInit(gameMem);
}

void GameUpdateAndRender(GameMemory *gameMem) {
    
    UpdateInput(&Game->inputQueue);

    InputQueue *input = &gameMem->inputQueue;

    if (InputPressed(input, Input_Escape)) {
        gameMem->running = false;
    }

    MosaicUpdateInternal();
    MosaicUpdate();
    MosaicRender();
    
    DeleteEntities(&Game->entityDB);

    gameMem->frame++;
    ClearFrameMem();
}
