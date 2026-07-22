
#define MOSAIC 0
#define MY_GAME 0
#define UI_TEST 1
#define NETWORK_SIMPLE 0


#if MOSAIC
#include "mosaic.cpp"

#elif MY_GAME
#include "game/my_game.cpp"

#elif UI_TEST
#include "examples/ui_test.cpp"

#elif NETWORK_SIMPLE
#include "examples/network_simple.cpp"

#endif

