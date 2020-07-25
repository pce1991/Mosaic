
// @TODO: I want to be able to specity a game to run in this file so I can provide numerous examples.

#define MESSAGING 0

#define VERSUS_PLATFORMER 1

#if MESSAGING
#include "examples/messaging_example.cpp"

#elif VERSUS_PLATFORMER
#include "examples/versus_platformer.cpp"

#endif
