#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <gl/glew.h>
#include <gl/wglew.h>

#include "windows_api.cpp"

#include <assert.h>
#define ASSERT(...) assert(__VA_ARGS__)

#include "game.cpp"

bool PlatformRunning = true;

struct OpenGLInfo {
    const uint8 *vendor;
    const uint8 *renderer;
    const uint8 *version;
    const uint8 *shadingLanguageVersion;
    const uint8 *extensions;
};



LRESULT CALLBACK MainWindowCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch(msg) {
        case WM_SIZE: {
        } break;

        case WM_DESTROY: {
            PlatformRunning = false;
        } break;

        case WM_CLOSE: {
            PlatformRunning = false;
        } break;

        case WM_QUIT: {
            PlatformRunning = false;
        } break;

        case WM_ACTIVATEAPP: {
        } break;

        // the default case means that if none of the other cases happened, we fall thru to this one
        // "Calls the default window procedure to provide default processing for any window messages
        // that an application does not process. This function ensures that every message is processed"
        default: {
            result = DefWindowProc(hwnd, msg, wParam, lParam);
        } break;
    }


    return result;
}

#if 1
void InitOpenGL(HWND window, OpenGLInfo *glInfo) {
    HDC deviceContext = GetDC(window);

    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.cDepthBits = 24;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

    int suggestedPixelFormatIndex = ChoosePixelFormat(deviceContext, &desiredPixelFormat);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(deviceContext, suggestedPixelFormatIndex,
                        sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(deviceContext, suggestedPixelFormatIndex, &suggestedPixelFormat);
    
    HGLRC renderContext = wglCreateContext(deviceContext);
    if (wglMakeCurrent(deviceContext, renderContext)) {

        {
            glInfo->vendor = glGetString(GL_VENDOR);
            glCheckError();
            glInfo->renderer = glGetString(GL_RENDERER);
            glCheckError();
            glInfo->version = glGetString(GL_VERSION);
            glCheckError();
            glInfo->shadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
            glCheckError();
            glInfo->extensions = glGetString(GL_EXTENSIONS);
            glCheckError();

            Print("%s", glInfo->vendor);
            Print("%s", glInfo->renderer);
            Print("%s", glInfo->version);
            Print("%s", glInfo->shadingLanguageVersion);
            Print("%s", glInfo->extensions);
        }

        GLenum err = glewInit();

        if (err == GLEW_OK) {

            ASSERT(WGLEW_ARB_create_context);

            int attribs[] = {
                             WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                             WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                             WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                             WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                             0
            };
                
            HGLRC sharedContext = 0;
            HGLRC modernContext = wglCreateContextAttribsARB(deviceContext, sharedContext, attribs);

            ASSERT(modernContext != NULL);
            wglMakeCurrent(deviceContext, modernContext);
            wglDeleteContext(renderContext);
            renderContext = modernContext;


            if (WGLEW_EXT_swap_control) {
                wglSwapIntervalEXT(1);
            }
        }
        else {
            Print((char *)glewGetErrorString(err));
        }
    }
}
#endif

void WindowsGetInput(InputQueue *inputQueue) {

    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

        switch (msg.message) {
            case WM_KEYDOWN: {
                int keycode = msg.wParam;

                if (keycode == 0x1B) {
                    PushInputPress(inputQueue, Input_Escape, 0);
                }

                if (keycode == 0x41) {
                    PushInputPress(inputQueue, Input_Left, 0);
                }

                if (keycode == 0x44) {
                    PushInputPress(inputQueue, Input_Right, 0);
                }

                if (keycode == 0x53) {
                    PushInputPress(inputQueue, Input_Down, 0);
                }

                if (keycode == 0x57) {
                    PushInputPress(inputQueue, Input_Up, 0);
                }

                if (keycode == VK_SPACE) {
                    PushInputPress(inputQueue, Input_Space, 0);
                }
            } break;

            case WM_KEYUP: {
                int keycode = msg.wParam;
                    
                if (keycode == 0x1B) {
                    PushInputRelease(inputQueue, Input_Escape, 0);
                }
                if (keycode == 0x41 || keycode == 0x4F) {
                    PushInputRelease(inputQueue, Input_Left, 0);
                }
                if (keycode == 0x44 || keycode == 0x55) {
                    PushInputRelease(inputQueue, Input_Right, 0);
                }
                if (keycode == 0x53) {
                    PushInputRelease(inputQueue, Input_Down, 0);
                }
                if (keycode == 0x57) {
                    PushInputRelease(inputQueue, Input_Up, 0);
                }

                if (keycode == VK_SPACE) {
                    PushInputRelease(inputQueue, Input_Space, 0);
                }

            } break;

            case WM_CHAR: {
                uint16 character = (uint16)msg.wParam;
            } break;

            case WM_MOUSEMOVE : {
                int posX = GET_X_LPARAM(msg.lParam);
                int posY = GET_Y_LPARAM(msg.lParam);

                inputQueue->mousePos.x = posX;
                inputQueue->mousePos.y = screenHeight - posY;
            } break;

                    
            case WM_LBUTTONDOWN : {
                PushInputPress(inputQueue, Input_MouseLeft, 0);
            } break;

            case WM_LBUTTONUP : {
                PushInputRelease(inputQueue, Input_MouseLeft, 0);
            } break;
                    
            case WM_RBUTTONDOWN : {
                PushInputPress(inputQueue, Input_MouseRight, 0);
            } break;

            case WM_RBUTTONUP : {
                PushInputRelease(inputQueue, Input_MouseRight, 0);
            } break;

            default : {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmndLine, int nCndShow) {

    char executablePath[MAX_PATH];

    GetModuleFileName(NULL, executablePath, sizeof(executablePath));
    char drive[MAX_PATH];
    char dir[MAX_PATH];
    _splitpath(executablePath, drive, dir, NULL, NULL);

    char newWorkingDirectory[MAX_PATH];
#if !ETC_RELEASE
    wsprintf(newWorkingDirectory, "%s%s..\\", drive, dir);
#else
    wsprintf(newWorkingDirectory, "%s%s", drive, dir);
#endif
    SetCurrentDirectory(newWorkingDirectory);




    WNDCLASS windowClass;
    memset(&windowClass, 0, sizeof(WNDCLASS));

    windowClass.lpszClassName = "OurGameWindowClass";

    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = MainWindowCallback;
    windowClass.hInstance = hInstance;
    windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    int registerResult = RegisterClass(&windowClass);
    if (registerResult == 0) {
        printf("There was an error registering the class");
        return 0;
    }

    
    DWORD dwExStyle = 0; WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX;

    RECT windowRect;
    windowRect.left = 0;
    windowRect.right = screenWidth;
    windowRect.top = 0;
    windowRect.bottom = screenHeight; 
    
    HWND window = CreateWindowEx(dwExStyle, windowClass.lpszClassName, "GAME",
                                 WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0, 0,
                                 NULL, NULL,
                                 hInstance, NULL);

    // @NOTE: the reason we do right - left, and bottom - top, is because AdjustWindowRect may change the
    // left/right/top/bottom coordinates of the rect, so the proper size is the difference between coordinates
    AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);
    
    SetWindowPos(window, HWND_NOTOPMOST,
                 64, 64,
                 windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                 SWP_NOZORDER);
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    OpenGLInfo glInfo;
    InitOpenGL(window, &glInfo);

    LARGE_INTEGER startSystemTime;
    LARGE_INTEGER systemTime;
    LARGE_INTEGER systemFrequency;
    QueryPerformanceFrequency(&systemFrequency);
    QueryPerformanceCounter(&systemTime);

    startSystemTime = systemTime;
    SeedRand(startSystemTime.QuadPart);

    // Init Game Memomry
    GameMemory gameMem;
    memset(&gameMem, 0, sizeof(GameMemory));

    // @GACK: need this for seeding the random number generator in GameInit
    // @BUG: It seems like the seeded value is almost always exactly the same tho???????
    gameMem.systemTime = (real32)systemTime.QuadPart;

    GameInit(&gameMem);

    gameMem.running = true;

    InputQueue *inputQueue = &gameMem.inputQueue;

    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd183376(v=vs.85).aspx
    BITMAPINFO bitmapInfo;
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = screenWidth;
    bitmapInfo.bmiHeader.biHeight = -screenHeight;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    // ALLOCATION/POINTERS
    int bitmapWidth = screenWidth;
    int bitmapHeight = screenHeight;
    int pixelCount = (bitmapWidth * bitmapHeight);
    int bytesPerPixel = 4; // one byte for each color
    int bitmapMemorySize = bytesPerPixel * pixelCount;

    // stack allocated variable
    gameMem.bitmap = (uint8 *)malloc(bitmapMemorySize);
    gameMem.pitch = 4 * bitmapWidth;

    
    HDC hdc = GetDC(window);

    real64 timeSinceRender = 0.0;

    ShowCursor(false);
    WinMoveMouse(window, screenWidth / 2.0f, screenHeight / 2.0f, screenHeight);

    while(gameMem.running && PlatformRunning) {

        LARGE_INTEGER prevSystemTime = systemTime;
        int32 error = QueryPerformanceCounter(&systemTime);

        gameMem.deltaTime = ((real64)systemTime.QuadPart - (real64)prevSystemTime.QuadPart) / (real64)systemFrequency.QuadPart;

        gameMem.time += gameMem.deltaTime;

        timeSinceRender += gameMem.deltaTime;

        ClearInputQueue(inputQueue);
        WindowsGetInput(inputQueue);

        // @TODO: use an actual accumulator
        if (timeSinceRender < FRAME_RATE) {
            real64 timeUntilRender = FRAME_RATE - timeSinceRender;
            //Sleep(timeUntilRender * 1000);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GameUpdateAndRender(&gameMem);

        HDC deviceContext = GetDC(window);
        SwapBuffers(deviceContext);
        ReleaseDC(window, deviceContext);
    }
    
    return 0;
}
