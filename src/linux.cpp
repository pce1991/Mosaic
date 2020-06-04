
#define LINUX 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <stdarg.h> 

#include <pthread.h>
#include <semaphore.h>

#include <assert.h>
#define ASSERT(...) assert(__VA_ARGS__)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define NUM_THREADS 4

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ETC_AUDIO_BUFFER_COUNT 4
//#include <alsa/asoundlib.h>

#include <linux/joystick.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include "types.h"

struct LinuxPlatform {
    Display *display;
    Window *window;
    int32 screenWidth;
    int32 screenHeight;
};

LinuxPlatform *Platform = NULL;

#include "linux_api.cpp"

#include "game.cpp"


#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef void (*glXSwapIntervalEXTProc)(Display*, GLXDrawable, int);

typedef struct {
    InputQueue *input;

    Display *display;

    sem_t *semaphore;

    int32 gamepadFileDescriptor;
} ReadInputProcData;

void PushKeyboardEvents(Display *display, InputQueue *inputQueue) {
    XEvent event;
    
    while (XPending(display) > 0) {

        XNextEvent(display, &event);

        if (event.type == KeyPress) {

            // @TODO: handle repeat keys
            if (event.xkey.keycode == 9) {
                PushInputPress(inputQueue, Input_Escape);
            }

            if (event.xkey.keycode == 36) {
                PushInputPress(inputQueue, Input_Return);
            }

            if (event.xkey.keycode == 22) {
                PushInputPress(inputQueue, Input_Backspace);
            }

            if (event.xkey.keycode == 65) {
                PushInputPress(inputQueue, Input_Space);
            }

            // @GACK @NOTE: dvorak and wasd bindings
            // maybe want an action mapping for input
            if (event.xkey.keycode == 113 ||
                event.xkey.keycode == 38 ||
                event.xkey.keycode == 32) {
                PushInputPress(inputQueue, Input_Left);
            }
            else if (event.xkey.keycode == 111) {
                PushInputPress(inputQueue, Input_Up);
            }
            else if (event.xkey.keycode == 114 ||
                     event.xkey.keycode == 40 ||
                     event.xkey.keycode == 30) {
                PushInputPress(inputQueue, Input_Right);
            }
            else if (event.xkey.keycode == 116) {
                PushInputPress(inputQueue, Input_Down);
            }
            else {
                KeySym keySym;
                char str[2];
                XLookupString(&event.xkey, str, 1, &keySym, NULL);
                str[1] = 0;

                // @TODO: support typing in characters
                /* // Right now ignore punctuation and capitilazation */
                /* if (IsLowercase(str[0]) || str[0] == 32) { */
                /*     PushInputChar(inputQueue, str[0]); */
                /* } */
            }
        }

        if (event.type == KeyRelease) {
            bool actuallyRelease = false;
                
            if (XEventsQueued(display, QueuedAfterReading)) {
                XEvent nextEvent;
                XPeekEvent(display, &nextEvent);

                if (nextEvent.type != KeyPress && nextEvent.xkey.time != event.xkey.time &&
                    nextEvent.xkey.keycode != event.xkey.keycode) {
                    actuallyRelease = true;
                }
            }
            else {
                actuallyRelease = true;
            }
                
            if (actuallyRelease) {
                if (event.xkey.keycode == 9) {
                    PushInputRelease(inputQueue, Input_Escape);
                }

                if (event.xkey.keycode == 36) {
                    PushInputRelease(inputQueue, Input_Return);
                }

                if (event.xkey.keycode == 22) {
                    PushInputRelease(inputQueue, Input_Backspace);
                }

                if (event.xkey.keycode == 65) {
                    PushInputRelease(inputQueue, Input_Space);
                }
                
                if (event.xkey.keycode == 113 ||
                    event.xkey.keycode == 38 ||
                    event.xkey.keycode == 32) {
                    PushInputRelease(inputQueue, Input_Left);
                }
                else if (event.xkey.keycode == 111) {
                    PushInputRelease(inputQueue, Input_Up);
                }
                else if (event.xkey.keycode == 114 ||
                         event.xkey.keycode == 40 ||
                         event.xkey.keycode == 30) {
                    PushInputRelease(inputQueue, Input_Right);
                }
                else if (event.xkey.keycode == 116) {
                    PushInputRelease(inputQueue, Input_Down);
                }
                
            }
        }
            

        if (event.type == MotionNotify) {
            inputQueue->mousePos.x = event.xmotion.x;
            inputQueue->mousePos.y = screenHeight - event.xmotion.y;
        }

        if (event.type == ButtonPress) {
            switch (event.xbutton.button) {
                case 1 : {
                    PushInputPress(inputQueue, Input_MouseLeft);
                } break;
                case 2 : {
                        
                } break;
                case 3 : {
                        
                } break;
            }
        }
        if (event.type == ButtonRelease) {
            switch (event.xbutton.button) {
                case 1 : {
                    PushInputRelease(inputQueue, Input_MouseLeft);
                } break;
                case 2 : {
                        
                } break;
                case 3 : {
                        
                } break;
            }
        }
    }    
}

void PushGamepadEvents(Display *display, InputQueue *inputQueue, int fileDescriptor) {
    struct js_event js_event = {};

    while (read(fileDescriptor, &js_event, sizeof(js_event)) > 0) {
        if (js_event.type == JS_EVENT_BUTTON) {

            // Press
            if (js_event.value == 1) {
                if (js_event.number <= 3) {
                    PushInputPress(inputQueue, Input_Space);
                }
            }
            // Release
            if (js_event.value == 0) {
                if (js_event.number <= 3) {
                    PushInputRelease(inputQueue, Input_Space);
                }
            }
        }

        if (js_event.type == JS_EVENT_AXIS) {
            if (js_event.number == 0) {
                if (js_event.value < -10000) {
                    PushInputPress(inputQueue, Input_Left);                    
                }
                else if (js_event.value > 10000) {
                    PushInputPress(inputQueue, Input_Right);                    
                }
                else {
                    PushInputRelease(inputQueue, Input_Left);
                    PushInputRelease(inputQueue, Input_Right);
                }
            }
            if (js_event.number == 1) {
                // ignore y input
            }
        }
    }
}

void* ReadInputProc(void *ptr) {
    ReadInputProcData *data = (ReadInputProcData *)ptr;
    InputQueue *inputQueue = (InputQueue *)data->input;
    Display *display = data->display;

    while(true) {
        if (sem_wait(data->semaphore) == 0) {
            PushKeyboardEvents(display, inputQueue);

            PushGamepadEvents(display, inputQueue, data->gamepadFileDescriptor);
        }
    }
}


void InitOpenGL(Display *display, Window *window, GLXContext *context) {

    static int visual_attribs[] = {
        GLX_X_RENDERABLE    , True,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_DIRECT_COLOR,
        GLX_DOUBLEBUFFER    , True,
        None
    };
  
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
        glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
  
    int fbCount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbCount);

    int bestFBC = -1, worstFBC = -1, bestNumSamp = -1, worstNumSamp = 999;

    // pick fb with most samples per pixel... why?
    for (int i = 0; i < fbCount; i++) {
        XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
        if (vi)
            {
                int samp_buf, samples;
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

                if ((bestFBC < 0 || samp_buf) && samples > bestNumSamp) {
                    bestFBC = i, bestNumSamp = samples;
                }
                if (worstFBC < 0 || !samp_buf || samples < worstNumSamp) {
                    worstFBC = i, worstNumSamp = samples;
                }
            }
        XFree(vi);
    }

    GLXFBConfig bestFbc = fbc[bestFBC];
    XFree(fbc);
  
    int contextAttributes[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
            GLX_CONTEXT_MINOR_VERSION_ARB, 1,
            //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };
    *context = glXCreateContextAttribsARB(display, bestFbc, 0, True, contextAttributes);
    glXMakeCurrent(display, *window, *context);

    
    // https://www.khronos.org/opengl/wiki/Swap_Interval#In_Linux_.2F_GLX
    // Your application's use of swap interval may be overridden by external, driver-specific configuration.
    // For example, forcing Vsync Off in a driver's control panel will prevent Vsync,
    // even if swap interval is set to 1 in your application.

    glXSwapIntervalEXTProc glXSwapIntervalEXT = 0;
    glXSwapIntervalEXT = (glXSwapIntervalEXTProc)
        glXGetProcAddressARB( (const GLubyte *) "glXSwapIntervalEXT" );
    //glXSwapIntervalEXT(display, *window, 1);

    // Must set to experimental so GLEW ignores glGetString and gets function pointers
    // https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW_.28OpenGL_Extension_Wrangler.29
    glewExperimental = GL_TRUE;
    glewInit();
    glCheckError();
}

int main() {

    Display *display;
    int screen;
    Window window;
    Window rootWindow;
    XSetWindowAttributes setAttributes;
    XWindowAttributes windowAttributes;
    
    GC context;

    // Boiler plate code
    int success = XInitThreads();

    display = XOpenDisplay(NULL);

    if (display == NULL) {
        printf("THERE WAS AN ERROR OPENING DISPLAY\n");
    }

    screen = DefaultScreen(display);
    rootWindow = RootWindow(display, screen);

    context = DefaultGC(display, screen);

    GLint glAttributes[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    XVisualInfo *visual = glXChooseVisual(display, screen, glAttributes);

    window = XCreateSimpleWindow(display, rootWindow, 10, 10, screenWidth, screenHeight, 1, XBlackPixel(display, screen), XWhitePixel(display, screen));
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | PointerMotionMask);

    XMapWindow(display, window);

    GLXContext glxContext = {};
    InitOpenGL(display, &window, &glxContext);

    int depth = DefaultDepth(display, screen);

    
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);

    // Milliseconds
    double startTime = (spec.tv_sec * 1000.0) + (spec.tv_nsec / 1.0e6);
    double gameTime = 0.0;
    double systemTime = startTime;
    double prevSystemTime = systemTime;
    double deltaTime;

    // seconds
    double frameRate = 1.0 / 60.0; // Hertz
    double timeSinceRender = 0.0;

    LinuxPlatform plat = {};
    plat.window = &window;
    plat.display = display;
    plat.screenWidth = screenWidth;
    plat.screenHeight = screenHeight;

    Platform = &plat;

    GameMemory gameMem = {};

    // @GACK: need this for seeding the random number generator in GameInit
    gameMem.systemTime = systemTime;
    SeedRand((int)systemTime);

    GameInit(&gameMem);

    gameMem.running = true;

    InputQueue *inputQueue = &gameMem.inputQueue;

    
    {
        pthread_t thread;

        sem_t semaphore;
        sem_init(&semaphore, false, 0);

        inputQueue->semaphore = &semaphore;

        ReadInputProcData data;
        data.input = inputQueue;
        data.display = display;
        data.semaphore = &semaphore;

        // @HACK: my keyboard is picked up as js0 for some reason, so we actually want to do the multiple gamepad
        // things, but also we have an issue where we'll push input twice for the keyboard if using js0
        data.gamepadFileDescriptor = open("/dev/input/js1", O_RDONLY | O_NONBLOCK);

        int tErr = pthread_create(&thread, NULL, ReadInputProc, &data);
        assert(tErr == 0);
    }


    while (gameMem.running) {

        clock_gettime(CLOCK_MONOTONIC, &spec);

        prevSystemTime = systemTime;
        systemTime = (spec.tv_sec * 1000.0) + (spec.tv_nsec / 1.0e6);

        deltaTime = (systemTime - prevSystemTime) / 1000.0;

        gameTime = (systemTime - startTime) / 1000.0;

        // seconds
        double dt = deltaTime;

        timeSinceRender += dt;

        gameMem.deltaTime = dt;
        gameMem.time = gameTime;
        gameMem.systemTime = systemTime;

        // @TODO: use an actual accumulator
        // @TODO: sleep AFTER the render
        // get difference from start of frame to end of frame
        // if our accumulator is > 0 then sleep
        // if we slept get dt again as time from end of render to end of sleep and accum -= dt
        // this is a better way to do it than sleeping all the time remaining because that isnt
        // guaranteed to wake up in time so we do small increments
        if (timeSinceRender < frameRate) {
            double timeUntilRender = frameRate - timeSinceRender;
            usleep(timeUntilRender * 1.0e6);
        }

        //printf("%d read %d keys\n", gameMem.frame, inputRead);

        GameUpdateAndRender(&gameMem);

        glXSwapBuffers(display, window);
        
        sem_post(inputQueue->semaphore);

        //XPutImage(display, window, context, image, 0, 0, 0, 0, screenWidth, screenHeight);

        timeSinceRender = 0.0;
    }

    return 0;
}
