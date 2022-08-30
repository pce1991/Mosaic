
struct Player {
    vec2i position;
};

struct Box {
    vec2i position;
};

struct UndoRecord {
    uint32 arenaHead;
    
    vec2i playerPosition;

    vec2i *boxPositions;
};

struct LevelState {
    Player player;

    int32 boxCount;
    Box *boxes;

    int32 undoRecordCapacity;
    int32 undoRecordCount;
    UndoRecord *undoRecords;
};

MemoryArena arena = {};
MemoryArena undoArena = {};

LevelState state = {};

void MyMosaicInit() {
    AllocateMemoryArena(&arena, Megabytes(64));
    AllocateMemoryArena(&undoArena, Megabytes(16));

    state.player.position = V2i(5, 5);

    state.boxes = PushArray(&arena, Box, 6);
    state.boxCount = 2;

    state.boxes[0].position = V2i(6, 6);
    state.boxes[1].position = V2i(4, 6);

    state.undoRecordCapacity = 2048;
    state.undoRecordCount = 0;
    state.undoRecords = PushArray(&arena, UndoRecord, state.undoRecordCapacity);
}

void MyMosaicUpdate() {

    ClearTiles(0, 0, 0);

    Player *player = &state.player;
    if (InputPressed(Input, Input_Space)) {
        // Create an undo record
        UndoRecord record = {};
        record.arenaHead = undoArena.size;
        record.playerPosition = player->position;
        record.boxPositions = PushArray(&undoArena, vec2i, state.boxCount);
        // @WARNING: because we are pushing a new size onto our memory arena
        // we want to remove it from the arena when we pop the undo record
        // otherwise our arena will just grow and grow even as our undo records
        // are getting popped off the stack.

        for (int i = 0; i < state.boxCount; i++) {
            Box *box = &state.boxes[i];
            record.boxPositions[i] = box->position;
        }

        player->position.x++;
        state.boxes[0].position.y++;
        state.boxes[1].position.x--;

        state.undoRecords[state.undoRecordCount] = record;
        state.undoRecordCount++;
    }

    if (InputPressed(Input, Input_Z)) {
        if (state.undoRecordCount > 0) {
            UndoRecord record = state.undoRecords[state.undoRecordCount - 1];
            undoArena.size = record.arenaHead;

            player->position = record.playerPosition;
            
            for (int i = 0; i < state.boxCount; i++) {
                Box *box = &state.boxes[i];
                box->position = record.boxPositions[i];
            }
        
            state.undoRecordCount--;
        }
    }

    SetTileColor(player->position.x, player->position.y, 1, 0, 0);

    for (int i = 0; i < state.boxCount; i++) {
        Box *box = &state.boxes[i];
        SetTileColor(box->position.x, box->position.y, 0, 1, 0);
    }
}

#if 0
MemoryArena arena = {};

int32 positionCount = 100;
vec2 *positions = NULL;

struct ParticleState {
    vec2 position;
    vec2 velocity;
    vec4 color;

    real32 lifetime;
    real32 duration;

    real32 timeOnChain;
    int32 velocityChainIndex;
};

struct ParticleSystem {
    int32 capacity;
    int32 count;

    ParticleState *particles;

    real32 spawnRate;
    int32 particlesPerSpawn;
    real32 spawnTimer;

    int32 chainLength;
    vec2 *velocityChain;
};

ParticleSystem particleSystem = {};



void UpdatePosition(vec2 position, vec2 velocity) {
    position.x += velocity.x;
    position.y += velocity.y;
}

void UpdatePosition(vec2 *position, vec2 velocity) {
    // the arrow dereferences a member variable of a struct,
    // it's the same thing as saying
    // int *p = ...;
    // *p = 5;

    (*position).x = 5;
    
    position->x += velocity.x;
    position->y += velocity.y;
}

vec2 RandomPointInCircle(vec2 center, real32 radius) {
    vec2 pt = {};
    real32 angle = RandfRange(0.0f, 6.28f);

    // Use the angle to generate a random direction on the unit circle.
    pt.x = cosf(angle);
    pt.y = sinf(angle);

    real32 r = RandfRange(0, radius);
    pt = pt * r;

    return pt + center;
}

// This function runs at the start of our program
void MyMosaicInit() {

    vec2 pos = V2(5, 5);
    vec2 vel = V2(10, 10);

    UpdatePosition(pos, vel);

    // This is going to update pos because the function is operating
    // on the memory location of pos, not a copy of the value
    UpdatePosition(&pos, vel);
    
    SetMosaicGridArray(200, 200);
    
    AllocateMemoryArena(&arena, Megabytes(1));

    positions = PushArray(&arena, vec2, positionCount);

    particleSystem.capacity = 5000;
    particleSystem.count = 0;
    particleSystem.particles = PushArray(&arena, ParticleState, particleSystem.capacity);

    particleSystem.spawnRate = 0.05f;
    particleSystem.particlesPerSpawn = 10;

    particleSystem.chainLength = 5;
    particleSystem.velocityChain = PushArray(&arena, vec2, particleSystem.chainLength);

    particleSystem.velocityChain[0] = V2(0.5f, -1) * 7;
    particleSystem.velocityChain[1] = V2(-0.5f, -1) * 7;
    particleSystem.velocityChain[2] = V2(-0.3f, -1) * 7;
    particleSystem.velocityChain[3] = V2(0.3f, -0.5f) * 7;
    particleSystem.velocityChain[4] = V2(1.7f, -0.5f) * 3;
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    particleSystem.spawnTimer += DeltaTime;

    if (particleSystem.spawnTimer > particleSystem.spawnRate) {
        particleSystem.spawnTimer -= particleSystem.spawnRate;
        
        for (int i = 0; i < particleSystem.particlesPerSpawn; i++) {
            // Every frame the particle system should spawn a particle.
            if (particleSystem.count < particleSystem.capacity) {
                ParticleState p = {};
                p.position = V2(100, 199);

                // Spawn the particle within a circle
                int32 s = RandiRange(0, 3);

                // if (s == 0) {
                //     p.position = RandomPointInCircle(V2(100, 100), 20);
                // }
                // if (s == 1) {
                //     p.position = RandomPointInCircle(V2(80, 80), 30);
                // }
                // if (s == 2) {
                //     p.position = RandomPointInCircle(V2(130, 110), 20);
                // }
                
                p.velocity = V2(RandfRange(-20, 20), -1);
                p.color = RGB(0.7f, 0.4f, 0.3f);

                p.color.r = RandfRange(0.3f, 0.8f);
                p.color.g = RandfRange(0.3f, 0.8f);
                p.color.b = RandfRange(0.3f, 0.8f);
                // p.color.g = p.color.r;
                // p.color.b = p.color.r;

                p.lifetime = 0;
                p.duration = 25.0f;

                p.velocityChainIndex = 0;
                p.velocity = particleSystem.velocityChain[0];
                p.velocity.x += RandfRange(-0.6f, 0.3f);
                p.velocity.y += RandfRange(-0.5f, 0.0f);

                particleSystem.particles[particleSystem.count] = p;
                particleSystem.count++;
            }
        }
    }
    

    for (int i = particleSystem.count - 1; i >= 0; i--) {
        // Instead of creating a copy I'm going to get a pointer
        //ParticleState p = particleSystem.particles[i];
        ParticleState *p = &particleSystem.particles[i];

        p->lifetime += DeltaTime;

        p->timeOnChain += DeltaTime;

        if (p->timeOnChain >= 5.5f) {
            p->velocityChainIndex++;

            p->timeOnChain = 0;

            if (p->velocityChainIndex > particleSystem.chainLength) {
                //p->velocityChainIndex
            }
            else {
                p->velocity = particleSystem.velocityChain[p->velocityChainIndex];
                // @HACK:
                p->velocity.x += RandfRange(-0.5, 0.5);
                p->velocity.y += RandfRange(-3, 0);
            }
        }

        if (p->lifetime > p->duration) {
            // Delete the particle by swapping the last element in the array
            particleSystem.particles[i] = particleSystem.particles[particleSystem.count - 1];
            particleSystem.count--;
        }
    }

    for (int i = 0; i < particleSystem.count; i++) {
        // Instead of creating a copy I'm going to get a pointer
        //ParticleState p = particleSystem.particles[i];
        ParticleState *p = &particleSystem.particles[i];

        // Update its velocity
        p->position.x += p->velocity.x * DeltaTime;
        p->position.y += p->velocity.y * DeltaTime;

        // Change the particle velocity when it reaches the next
        // point in the chain.

        SetTileColor(p->position, p->color);
    }
}
#endif
