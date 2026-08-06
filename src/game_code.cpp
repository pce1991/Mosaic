
#define MOSAIC 1
#define MY_GAME 0
#define SPRITE_TEST 1
#define UI_TEST 1
#define UI_TEST2 0
#define NETWORK_SIMPLE 0


#if MOSAIC
#include "mosaic.cpp"

#elif SPRITE_TEST
#include "examples/sprite_basics.cpp"

#elif MY_GAME
#include "game/my_game.cpp"

#elif UI_TEST
#include "examples/ui_test.cpp"

#elif UI_TEST2
#include "examples/ui_test2.cpp"

#elif NETWORK_SIMPLE
#include "examples/network_simple.cpp"

#endif

