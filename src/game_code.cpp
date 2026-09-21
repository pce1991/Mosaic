
#define MOSAIC 0
#define MY_GAME 0
#define UI_TEST 0
#define UI_TEST2 0
#define NETWORK_SIMPLE 0
#define EDITOR_SIMPLE 0
#define MULTITHREAD_TESTA 1
#define MULTITHREAD_TESTB 0



#if MOSAIC
#include "mosaic.cpp"

#elif MY_GAME
#include "game/my_game.cpp"

#elif UI_TEST
#include "examples/ui_test.cpp"

#elif UI_TEST2
#include "examples/ui_test2.cpp"

#elif NETWORK_SIMPLE
#include "examples/network_simple.cpp"

#elif EDITOR_SIMPLE
#include "examples/editor_simple.cpp"

#elif MULTITHREAD_TESTA
#include "examples/multithreading.cpp"


#endif

