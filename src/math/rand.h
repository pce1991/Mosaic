
// found that Microsoft used this for C++ lib in 2005, seems fine
#define LCG_MULT 214013u
#define LCG_INC 2531011u

// an alternative
// #define LCG_MULT 1664525u
// #define LCG_INC 1013904223u

typedef struct {
    uint32 seed;
    uint32 state;
} LCGState;

LCGState defaultLCGState = {};

void SeedRand(uint32 seed) {
    defaultLCGState.seed = seed;
    defaultLCGState.state = seed;
}

uint32 Randi() {
    defaultLCGState.state = defaultLCGState.state * LCG_MULT + LCG_INC;
    return defaultLCGState.state;
}

uint32 Randi(uint32 upperLimit) {
  // we use the high-bits we get from Randi which have better distribution
  // and then map those onto our correct range.
  return (uint32)(((uint64)Randi() * upperLimit) >> 32);
}

int32 RandiRange(int32 lowerLimit, int32 upperLimit) {
    return lowerLimit + (int32)Randi((uint32)(upperLimit - lowerLimit));
}

float32 Randf() {
    return (float32)Randi() / (float32)UINT32_MAX;
}

float32 RandfUpper(float32 upperLimit) {
    return Randf() * upperLimit;
}

float32 RandfRange(float32 lowerLimit, float32 upperLimit) {
    return lowerLimit + RandfUpper(upperLimit - lowerLimit);
}


void SeedRand(LCGState *state, uint32 seed) {
    state->seed = seed;
    state->state = seed;
}

uint32 Randi(LCGState *state) {
    state->state = state->state * LCG_MULT + LCG_INC;
    return state->state;
}

uint32 Randi(LCGState *state, uint32 upperLimit) {
  return Randi(state) % upperLimit;
}

int32 RandiRange(LCGState *state, int32 lowerLimit, int32 upperLimit) {
  return lowerLimit + (int32)Randi(state, (uint32)(upperLimit - lowerLimit));
}

float32 Randf(LCGState *state) {
  return (float32)Randi(state) / (float32)UINT32_MAX;
}

float32 RandfUpper(LCGState *state, float32 upperLimit) {
    return Randf(state) * upperLimit;
}

float32 RandfRange(LCGState *state, float32 lowerLimit, float32 upperLimit) {
  return lowerLimit + RandfUpper(state, upperLimit - lowerLimit);
}

