
// @TODO: I want to be able to specity a game to run in this file so I can provide numerous examples.

#define MESSAGING 0

#define PONG_BAD 0

#define PARTICLES 1

#if MESSAGING
#include "examples/messaging_example.cpp"

#elif PONG_BAD
#include "examples/pong_bad.cpp"

#elif PARTICLES
#include "examples/pretty.cpp"

#endif
