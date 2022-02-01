
#define MOSAIC 0
#define MY_GAME 1

#define NETWORK_SIMPLE 1
#define MESSAGING 1
#define PONG_BAD 0
#define PARTICLES 0
#define TYPING 0
#define COLLISION 0
#define SPRITE 0
#define ENTITY 0

#define MATRIX_EX 0

#define RENDERING 0

#define PARSING 0

#define DX12_TEST 1
#define RAYTRACER 1

#define PARSING 0

#define BABY 1

#if MOSAIC
#include "mosaic.cpp"

#elif MY_GAME
#include "examples/my_game.cpp"

#elif NETWORK_SIMPLE
#include "examples/network_simple.cpp"

#elif MESSAGING
#include "examples/messaging_example.cpp"

#elif PONG_BAD
#include "examples/pong_bad.cpp"

#elif PARTICLES
#include "examples/pretty.cpp"

#elif TYPING
#include "examples/typing_game.cpp"

#elif COLLISION
#include "examples/collision_example.cpp"

#elif SPRITE
#include "examples/sprite_basics.cpp"

#elif ENTITY
#include "examples/entity_example.cpp"

#elif RENDERING
#include "examples/rendering_example.cpp"

#elif MATRIX_EX
#include "examples/matrix_examples.cpp"

#elif PARSING
#include "examples/parsing_example.cpp"

#elif DX12_TEST
#include "examples/dx12_test.cpp"

#elif BABY
#include "examples/baby_demo.cpp"

#elif RAYTRACER
#include "examples/raytracing.cpp"


#endif

