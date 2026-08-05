
#define LINUX 1

#define OPENGL 1
#define DX12 0
#define DX11 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>
#include <semaphore.h>

#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

// @NOTE: EGL must be included before GLEW so that GLEW can load function
// pointers through eglGetProcAddress instead of glXGetProcAddress.
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GL/glew.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include "../types.h"

struct LinuxPlatform {
    Display *display;
    Window window;
    int32 screenWidth;
    int32 screenHeight;

    Atom wmDeleteWindow;
};

LinuxPlatform *Platform = NULL;

bool PlatformRunning = true;

#include "linux_api.cpp"

#include <assert.h>
#define ASSERT(...) assert(__VA_ARGS__)

#include "../game.cpp"

#include "linux_input.cpp"


struct OpenGLInfo {
    const uint8 *vendor;
    const uint8 *renderer;
    const uint8 *version;
    const uint8 *shadingLanguageVersion;
    const uint8 *extensions;
};

#define AUDIO_SAMPLERATE 48000
#define AUDIO_NUM_CHANNELS 2
#define AUDIO_BITS_PER_SAMPLE 32
#define AUDIO_BUFFER_SIZE_FRAMES 4800 // 100ms

struct LinuxAudioOutput {
    snd_pcm_t *pcm;
};

struct GamePlatform {
    bool running;
    LinuxAudioOutput audio;

    CoreMemory gameMem;
};


void InitOpenGL(Display *display, int32 screenWidth, int32 screenHeight, LinuxPlatform *platform,
                EGLDisplay *eglDisplayOut, EGLSurface *eglSurfaceOut, EGLContext *eglContextOut,
                OpenGLInfo *glInfo) {
    int screen = DefaultScreen(display);
    Window rootWindow = RootWindow(display, screen);

    EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);
    if (eglDisplay == EGL_NO_DISPLAY) {
        Print("Error: eglGetDisplay failed");
        return;
    }

    EGLint major, minor;
    if (!eglInitialize(eglDisplay, &major, &minor)) {
        Print("Error: eglInitialize failed");
        return;
    }

    eglBindAPI(EGL_OPENGL_API);

    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    EGLint configCount;
    EGLConfig config;
    if (!eglChooseConfig(eglDisplay, configAttribs, &config, 1, &configCount) || configCount < 1) {
        Print("Error: eglChooseConfig failed");
        return;
    }

    // Get the X visual that matches our EGL config so the window and the
    // EGL surface are compatible.
    XVisualInfo *visInfo = NULL;
    bool heapVisInfo = false;

    EGLint nativeVisualID = 0;
    eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &nativeVisualID);

    if (nativeVisualID != 0) {
        XVisualInfo visTemplate;
        visTemplate.visualid = nativeVisualID;
        int count = 0;
        visInfo = XGetVisualInfo(display, VisualIDMask, &visTemplate, &count);
        heapVisInfo = (visInfo != NULL);
    }

    XVisualInfo defaultVis;
    if (visInfo == NULL) {
        // @NOTE: fall back to the default screen visual
        defaultVis.visual = DefaultVisual(display, screen);
        defaultVis.depth = DefaultDepth(display, screen);
        visInfo = &defaultVis;
    }

    Colormap colormap = XCreateColormap(display, rootWindow, visInfo->visual, AllocNone);

    XSetWindowAttributes setAttributes = {};
    setAttributes.colormap = colormap;
    setAttributes.background_pixel = 0;
    setAttributes.border_pixel = 0;
    setAttributes.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask |
                               ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

    Window window = XCreateWindow(display, rootWindow,
                                  0, 0, screenWidth, screenHeight, 0,
                                  visInfo->depth, InputOutput, visInfo->visual,
                                  CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
                                  &setAttributes);

    XStoreName(display, window, "GAME");

    // @NOTE: the windows layer uses a fixed size, non-resizable window, so do the same here.
    XSizeHints *sizeHints = XAllocSizeHints();
    sizeHints->flags = PMinSize | PMaxSize;
    sizeHints->min_width = sizeHints->max_width = screenWidth;
    sizeHints->min_height = sizeHints->max_height = screenHeight;
    XSetWMNormalHints(display, window, sizeHints);
    XFree(sizeHints);

    // @NOTE: lets us catch the window close button (like WM_CLOSE on windows).
    platform->wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &platform->wmDeleteWindow, 1);

    XMapWindow(display, window);
    XFlush(display);

    if (heapVisInfo) {
        XFree(visInfo);
    }

    platform->window = window;

    EGLSurface eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)window, NULL);
    if (eglSurface == EGL_NO_SURFACE) {
        Print("Error: eglCreateWindowSurface failed");
        return;
    }

    EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 3,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE
    };

    EGLContext eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    if (eglContext == EGL_NO_CONTEXT) {
        Print("Error: eglCreateContext failed");
        return;
    }

    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        Print("Error: eglMakeCurrent failed");
        return;
    }

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

    // @NOTE: must be set to experimental so GLEW ignores glGetString and gets
    // function pointers when running against a core profile context.
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        Print((char *)glewGetErrorString(glewError));
    }

    // vsync
    eglSwapInterval(eglDisplay, 1);

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    *eglDisplayOut = eglDisplay;
    *eglSurfaceOut = eglSurface;
    *eglContextOut = eglContext;
}


void InitALSA(LinuxAudioOutput *audio) {
    int32 err;

    err = snd_pcm_open(&audio->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        Print("snd_pcm_open error: %s", snd_strerror(err));
        audio->pcm = NULL;
        return;
    }

    snd_pcm_hw_params_t *hwParams;
    snd_pcm_hw_params_alloca(&hwParams);

    snd_pcm_hw_params_any(audio->pcm, hwParams);

    err = snd_pcm_hw_params_set_access(audio->pcm, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    err = snd_pcm_hw_params_set_format(audio->pcm, hwParams, SND_PCM_FORMAT_FLOAT_LE);
    err = snd_pcm_hw_params_set_channels(audio->pcm, hwParams, AUDIO_NUM_CHANNELS);

    uint32 rate = AUDIO_SAMPLERATE;
    err = snd_pcm_hw_params_set_rate_near(audio->pcm, hwParams, &rate, 0);

    snd_pcm_uframes_t bufferFrames = AUDIO_BUFFER_SIZE_FRAMES;
    err = snd_pcm_hw_params_set_buffer_size_near(audio->pcm, hwParams, &bufferFrames);

    err = snd_pcm_hw_params(audio->pcm, hwParams);
    if (err < 0) {
        Print("snd_pcm_hw_params error: %s", snd_strerror(err));
        return;
    }

    snd_pcm_sw_params_t *swParams;
    snd_pcm_sw_params_alloca(&swParams);
    snd_pcm_sw_params_current(audio->pcm, swParams);
    snd_pcm_sw_params_set_start_threshold(audio->pcm, swParams, bufferFrames / 2);
    snd_pcm_sw_params(audio->pcm, swParams);

    snd_pcm_prepare(audio->pcm);
}

void ALSAThreadProc(void *data) {
    GamePlatform *platform = (GamePlatform *)data;
    LinuxAudioOutput *audio = &platform->audio;

    if (audio->pcm == NULL) {
        return;
    }

    real32 buffer[AUDIO_BUFFER_SIZE_FRAMES * AUDIO_NUM_CHANNELS];
    int32 framesToRender = AUDIO_BUFFER_SIZE_FRAMES;

    while (platform->running) {
        WriteSoundSamples(&platform->gameMem, framesToRender, buffer);

        snd_pcm_sframes_t written = snd_pcm_writei(audio->pcm, buffer, framesToRender);
        if (written < 0) {
            // @NOTE: snd_pcm_recover handles xruns (EPIPE) and other recoverable errors.
            snd_pcm_recover(audio->pcm, (int)written, 1);
        }
    }
}

void StartALSAThread(GamePlatform *platform) {
    pthread_t threadHandle;
    int err = pthread_create(&threadHandle, NULL, (void *(*)(void *))ALSAThreadProc, platform);
    if (err != 0) {
        Print("Error creating audio thread: %d", err);
        return;
    }
    pthread_detach(threadHandle);
}


// @NOTE: this lets us run the game from anywhere and still find the shaders
// and data folders. In dev builds we go up one directory (mirroring the
// windows layer's #if !ETC_RELEASE behavior).
void SetWorkingDirectory() {
    char executablePath[4096];
    ssize_t len = readlink("/proc/self/exe", executablePath, sizeof(executablePath) - 1);
    if (len < 0) {
        return;
    }
    executablePath[len] = 0;

    char *lastSlash = strrchr(executablePath, '/');
    if (lastSlash == NULL) {
        return;
    }
    *lastSlash = 0;

    char newWorkingDirectory[4096];
#if !ETC_RELEASE
    snprintf(newWorkingDirectory, sizeof(newWorkingDirectory), "%s/..", executablePath);
#else
    snprintf(newWorkingDirectory, sizeof(newWorkingDirectory), "%s", executablePath);
#endif
    chdir(newWorkingDirectory);
}


int main() {
    GamePlatform platform = {};
    platform.running = true;

    SetWorkingDirectory();

    LinuxPlatform plat = {};
    Platform = &plat;

    // Init Game Memory
    CoreMemory *gameMem = &platform.gameMem;
    memset(gameMem, 0, sizeof(CoreMemory));

    Core = gameMem;

    AllocateMemoryArena(&Core->permanentArena, Megabytes(256));
    AllocateMemoryArena(&Core->frameMem, Megabytes(32));

    bool gotConfigFile = ReadConfigFile("config.m_txt");

    if (!gotConfigFile) {
        Core->graphics.screenWidth = 1600;
        Core->graphics.screenHeight = 900;

        Core->audioPlayer.volume = 1.0f;
        Core->networkInfo.serverIPString = "192.0.0.1"; // @NOTE: this is just the IP address referring to yourself

        // @TODO: write out a config file if there isnt one already
    }

    plat.screenWidth = gameMem->graphics.screenWidth;
    plat.screenHeight = gameMem->graphics.screenHeight;

    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        Print("THERE WAS AN ERROR OPENING DISPLAY");
        return 0;
    }
    plat.display = display;

    OpenGLInfo glInfo;
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;
    EGLContext eglContext;
    InitOpenGL(display, plat.screenWidth, plat.screenHeight, &plat,
               &eglDisplay, &eglSurface, &eglContext, &glInfo);

    InitALSA(&platform.audio);

    StartALSAThread(&platform);

    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    double startSystemTime = (double)spec.tv_sec + (double)spec.tv_nsec / 1.0e9;

    SeedRand((uint32)((uint64)spec.tv_sec * 1000000000ULL + (uint64)spec.tv_nsec));

    // @GACK: need this for seeding the random number generator in GameInit
    gameMem->systemTime = (real32)startSystemTime;

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

    MoveMouse(Core->graphics.screenWidth / 2.0f, Core->graphics.screenHeight / 2.0f);

    gameMem->systemTime = (real32)startSystemTime;

    Print("Starting Game Loop");
    Print("");

    double prevSystemTime = startSystemTime;
    real64 timeSinceRender = 0.0;

    while (gameMem->running && PlatformRunning) {

        clock_gettime(CLOCK_MONOTONIC, &spec);
        double systemTime = (double)spec.tv_sec + (double)spec.tv_nsec / 1.0e9;

        gameMem->deltaTime = (real32)(systemTime - prevSystemTime);
        gameMem->time += gameMem->deltaTime;

        Time = gameMem->time;
        DeltaTime = gameMem->deltaTime;

        timeSinceRender += gameMem->deltaTime;

        ClearInputManager(inputManager);
        LinuxGetInput(inputManager);

        // @TODO: use an actual accumulator
        if (timeSinceRender < FRAME_RATE) {
            real64 timeUntilRender = FRAME_RATE - timeSinceRender;
            //usleep(timeUntilRender * 1.0e6);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GameUpdateAndRender(gameMem);

        eglSwapBuffers(eglDisplay, eglSurface);
    }

    GameDeinit();

    // @NOTE: the audio thread is left running; the process exits and tears it down,
    // which mirrors the windows layer leaking its WASAPI thread/objects.
    return 0;
}
