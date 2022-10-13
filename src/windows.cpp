#define WINDOWS 1

#define OPENGL 1
#define DX12 0
#define DX11 0

#include <ws2tcpip.h>
#include <Mstcpip.h> // only need this if turning on promiscuous mode for socket.

#include <windows.h>
#include <windowsx.h>


#include <mmdeviceapi.h>
#include <audioclient.h>

#include <stdio.h>
#include <stdlib.h>

#if OPENGL
#define GLEW_STATIC
#include <gl/glew.h>
#include <gl/wglew.h>
#elif DX12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h> // must link the d3dcompiler_47.dll in lib/dx12
//#include <DirectXMath.h>
//#include "dx12/d3dx12.h"
#endif


#include "types.h"

struct WindowsPlatform {
    HWND *window;
    int32 screenHeight;
    int32 screenWidth;

    WSADATA wsaData;
};

WindowsPlatform *Platform = NULL;

#include "windows_api.cpp"

#include <assert.h>
#define ASSERT(...) assert(__VA_ARGS__)

#include "game.cpp"

#include "windows_input.cpp"



bool PlatformRunning = true;


struct OpenGLInfo {
    const uint8 *vendor;
    const uint8 *renderer;
    const uint8 *version;
    const uint8 *shadingLanguageVersion;
    const uint8 *extensions;
};

#define AUDIO_SAMPLERATE 44100
#define AUDIO_NUM_CHANNELS 2
#define AUDIO_BITS_PER_SAMPLE 32

struct WinAudioOutput {
    IMMDevice *device;
    IAudioClient *audioClient;
    IAudioRenderClient *renderClient;
    HANDLE renderEvent;
    HANDLE audioSema;

    WAVEFORMATEX waveFormat;

    uint32 bufferSampleCount;
    uint32 samplesRendered;
};

struct GamePlatform {
    bool running;
    WinAudioOutput audio;

    GameMemory gameMem;
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

        // the default case means that if none of the other cases happened, we fall thru
        // to this one "Calls the default window procedure to provide default processing
        // for any window messages that an application does not process. This function
        // ensures that every message is processed"
        default: {
            result = DefWindowProc(hwnd, msg, wParam, lParam);
        } break;
    }


    return result;
}

#if OPENGL
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
                             WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
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
                // vsync
                wglSwapIntervalEXT(1);
            }
        }
        else {
            Print((char *)glewGetErrorString(err));
        }
    }

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
}
#endif


void InitWASAPI(WinAudioOutput *audio) {
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    HANDLE renderEvent;

    uint32 actualBufferSampleCount;

    REFERENCE_TIME bufferDuration = 1000000;

    HRESULT result;
    result = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (SUCCEEDED(result)) {

        // @TODO: make sure to close this device later!
        result = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
        if (SUCCEEDED(result)) {
            result = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
            if (SUCCEEDED(result)) {

                WAVEFORMATEX *waveFormat = &audio->waveFormat;
                waveFormat->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
                waveFormat->nChannels = AUDIO_NUM_CHANNELS;
                waveFormat->nSamplesPerSec = AUDIO_SAMPLERATE;
                waveFormat->wBitsPerSample = AUDIO_BITS_PER_SAMPLE;
                waveFormat->nBlockAlign = (waveFormat->nChannels * waveFormat->wBitsPerSample) / 8;
                waveFormat->nAvgBytesPerSec = waveFormat->nSamplesPerSec * waveFormat->nBlockAlign;
                waveFormat->cbSize = 0;

                WAVEFORMATEXTENSIBLE *closestWaveFormat;
                result = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, waveFormat, (WAVEFORMATEX **)&closestWaveFormat);

                if (result == S_FALSE) {
                    if (closestWaveFormat->Format.nSamplesPerSec != waveFormat->nSamplesPerSec) {
                        // @TODO: need to resample, there appears to be an API...
                        // https://msdn.microsoft.com/en-us/library/windows/desktop/ff819070(v=vs.85).aspx
                        // https://sourceforge.net/p/playpcmwin/wiki/HowToUseResamplerMFT/
                    }

                    waveFormat = (WAVEFORMATEX *)closestWaveFormat;
                }
                    
                result = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                                  AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                                  bufferDuration, 0, waveFormat, NULL);
                if (SUCCEEDED(result)) {

                    result = pAudioClient->GetBufferSize(&actualBufferSampleCount);
                    if (SUCCEEDED(result)) {

                        result = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pRenderClient);
                        if (SUCCEEDED(result)) {

                            renderEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
                            result = pAudioClient->SetEventHandle(renderEvent);
                            if (SUCCEEDED(result)) {
                                // @NOTE: success!
                            }
                        }
                    }
                }
            }
        }
    }

    // @TODO: LOG errors if there were any!!!

    audio->device = pDevice;
    audio->audioClient = pAudioClient;
    audio->renderClient = pRenderClient;
    audio->renderEvent = renderEvent;
    audio->bufferSampleCount = actualBufferSampleCount;

    audio->audioSema = CreateSemaphoreEx(NULL, 1, 1, NULL, 0, SEMAPHORE_ALL_ACCESS);
}

void WASAPIThreadProc(void *data) {
    CoInitialize(NULL);

    GamePlatform *platform = (GamePlatform *)data;
    WinAudioOutput *audio = &platform->audio;

    if (audio->audioClient == NULL) {
        return;
    }

    audio->audioClient->Start();

    uint32 framesMissed = 0;

    while (platform->running) {
        WaitForSingleObject(audio->renderEvent, INFINITE);

        uint32 numFramesPadding;
        if (SUCCEEDED(audio->audioClient->GetCurrentPadding(&numFramesPadding))) {
            uint32 numFramesAvailable = audio->bufferSampleCount - numFramesPadding;

            if (numFramesAvailable == 0) {
                continue;
            }

            real32 *buffer;
            if (SUCCEEDED(audio->renderClient->GetBuffer(numFramesAvailable, (BYTE **)&buffer))) {

                if (WaitForSingleObject(audio->audioSema, 0) == WAIT_OBJECT_0) {

                    while (framesMissed > 0) {
                        uint32 framesToRender = Min(numFramesAvailable, framesMissed);
                        WriteSoundSamples(&platform->gameMem, framesToRender, buffer);
                        framesMissed -= framesToRender;
                    }
                    
                    WriteSoundSamples(&platform->gameMem, numFramesAvailable, buffer);
                    ReleaseSemaphore(audio->audioSema, 1, NULL);
                }
                else {
                    memset(buffer, 0, sizeof(real32) * 2 * numFramesAvailable);
                    framesMissed += numFramesAvailable;
                }

                audio->renderClient->ReleaseBuffer(numFramesAvailable, 0);
            }
        }
    }

    CoUninitialize();
}

void StartWASAPIThread(GamePlatform *platform) {
    DWORD threadID;
    HANDLE threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WASAPIThreadProc, platform, 0, &threadID);
    CloseHandle(threadHandle);
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmndLine, int nCndShow) {
    CoInitialize(NULL);

    GamePlatform platform = {};
    platform.running = true;

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

    
    WindowsPlatform plat = {};

    Platform = &plat;
    
    // Init Game Memomry
    GameMemory *gameMem = &platform.gameMem;
    memset(gameMem, 0, sizeof(GameMemory));

    Game = gameMem;

    plat.screenWidth = gameMem->screenWidth;
    plat.screenHeight = gameMem->screenHeight;

    bool gotConfigFile = ReadConfigFile("config.m_txt");

    if (!gotConfigFile) {
        Game->screenWidth = 1600;
        Game->screenHeight = 900;

        Game->audioPlayer.volume = 1.0f;
        Game->networkInfo.serverIPString = "192.0.0.1"; // @NOTE: this is just the IP address referring to yourself

        // @TODO: write out a config file if there isnt one already
    }

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
    windowRect.right = Game->screenWidth;
    windowRect.top = 0;
    windowRect.bottom = Game->screenHeight; 
    
    HWND window = CreateWindowEx(dwExStyle, windowClass.lpszClassName, "GAME",
                                 WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 0, 0,
                                 NULL, NULL,
                                 hInstance, NULL);

    plat.window = &window;

    // @NOTE: the reason we do right - left, and bottom - top, is because AdjustWindowRect may change the
    // left/right/top/bottom coordinates of the rect, so the proper size is the difference between coordinates
    AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);
    
    SetWindowPos(window, HWND_NOTOPMOST,
                 0, 0,
                 windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                 SWP_NOZORDER);
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);


#if OPENGL
    OpenGLInfo glInfo;
    InitOpenGL(window, &glInfo);
#elif DX12
    
#endif

    InitWASAPI(&platform.audio);

    StartWASAPIThread(&platform);

    LARGE_INTEGER startSystemTime;
    LARGE_INTEGER systemTime;
    LARGE_INTEGER systemFrequency;
    QueryPerformanceFrequency(&systemFrequency);
    QueryPerformanceCounter(&systemTime);

    startSystemTime = systemTime;
    SeedRand(startSystemTime.QuadPart);

    
    WSAStartup(MAKEWORD(2,2), &Platform->wsaData);

    // @GACK: need this for seeding the random number generator in GameInit
    // @BUG: It seems like the seeded value is almost always exactly the same tho?
    gameMem->systemTime = (real32)systemTime.QuadPart;

    GameInit(gameMem);
    gameMem->startTime = 0.0f;

    gameMem->running = true;

    InputManager *inputManager = &gameMem->inputManager;
    AllocateInputManager(inputManager, &gameMem->permanentArena, 32, 4);
    gameMem->keyboard = &inputManager->devices[0];
    gameMem->mouse = &inputManager->devices[1];

    AllocateInputDevice(gameMem->keyboard, InputDeviceType_Keyboard, Input_KeyboardDiscreteCount, 0);
    AllocateInputDevice(gameMem->mouse, InputDeviceType_Mouse, Input_MouseDiscreteCount, Input_MouseAnalogueCount);

    Keyboard = gameMem->keyboard;
    Mouse = gameMem->mouse;

    // Used for software rendering.
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd183376(v=vs.85).aspx
    BITMAPINFO bitmapInfo;
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = Game->screenWidth;
    bitmapInfo.bmiHeader.biHeight = -Game->screenHeight;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    // ALLOCATION/POINTERS
    int bitmapWidth = Game->screenWidth;
    int bitmapHeight = Game->screenHeight;
    int pixelCount = (bitmapWidth * bitmapHeight);
    int bytesPerPixel = 4; // one byte for each color
    int bitmapMemorySize = bytesPerPixel * pixelCount;

    
    HDC hdc = GetDC(window);

    real64 timeSinceRender = 0.0;

    HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
    SetCursor(cursor);
    //ShowCursor(false);
    WinMoveMouse(window, Game->screenWidth / 2.0f, Game->screenHeight / 2.0f, Game->screenHeight);

    
    gameMem->systemTime = (real32)systemTime.QuadPart;

    while(gameMem->running && PlatformRunning) {

        LARGE_INTEGER prevSystemTime = systemTime;
        int32 error = QueryPerformanceCounter(&systemTime);

        gameMem->deltaTime = ((real64)systemTime.QuadPart - (real64)prevSystemTime.QuadPart) / (real64)systemFrequency.QuadPart;

        gameMem->time += gameMem->deltaTime;

        Time = gameMem->time;
        DeltaTime = gameMem->deltaTime;

        timeSinceRender += gameMem->deltaTime;

        ClearInputManager(inputManager);
        WindowsGetInput(inputManager);

        // @TODO: use an actual accumulator
        if (timeSinceRender < FRAME_RATE) {
            real64 timeUntilRender = FRAME_RATE - timeSinceRender;
            //Sleep(timeUntilRender * 1000);
        }

#if OPENGL
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif

        GameUpdateAndRender(gameMem);

        SwapBuffers(hdc);
    }

    GameDeinit();

    WSACleanup();
    
    return 0;
}
