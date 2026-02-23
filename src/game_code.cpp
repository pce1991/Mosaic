
#define MOSAIC 1
#define MY_GAME 0

#define NETWORK_SIMPLE 1


#if MOSAIC
#include "mosaic.cpp"

#elif MY_GAME
#include "game/my_game.cpp"

#elif NETWORK_SIMPLE
#include "examples/network_simple.cpp"

#endif

