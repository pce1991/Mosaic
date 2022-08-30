
MemoryArena arena = {};

struct ParticleEmitter {
    vec2 position;
    int32 maxCount;
    int32 count;
    real32 spawnRate;
    real32 spawnTimer;

    real32 duration;
};

struct ParticleState {
    vec2 position;
    vec2 velocity;
    vec4 color;
    real32 lifetime;
    real32 duration;

    ParticleEmitter *emitter;
};

struct ParticleBuffer {
    int32 capacity;
    int32 count;
    ParticleState *states;
};

ParticleBuffer buffer = {};
    
ParticleEmitter emitters[2];

void MyMosaicInit() {
    SetMosaicGridSize(64, 64);
    
    AllocateMemoryArena(&arena, Megabytes(64));

    buffer.capacity = 10000;
    buffer.states = PushArray(&arena, ParticleState, buffer.capacity);

    emitters[0].maxCount = 50;
    emitters[0].count = 0;
    emitters[0].spawnRate = 0.1;
    emitters[0].position = V2(31, 31);
    emitters[0].duration = 1.0f;

    emitters[1].maxCount = 100;
    emitters[1].count = 0;
    emitters[1].spawnRate = 0.2;
    emitters[1].position = V2(16, 60);
    emitters[1].duration = 5.0f;
}


void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    for (int i = 0; i < 2; i++) {
        ParticleEmitter *emitter = &emitters[i];

        emitter->spawnTimer += DeltaTime;
        
        if (emitter->spawnTimer > emitter->spawnRate) {
            if (emitter->count + 1 < emitter->maxCount) {
                ParticleState *p = &buffer.states[buffer.count];

                p->emitter = emitter;
            
                p->lifetime = 0;
                p->position = emitter->position;

                p->velocity = V2(RandfRange(-1, 1), -1);
                p->velocity = p->velocity * 10;

                p->duration = emitter->duration;
        
                p->color = V4(0.7f, 0.2f, 0.2f, 1.0f);

                buffer.count++;
                emitter->count++;
            }
        }
    }

    // Update
    //for (int i = 0; i < emitter.count; i++) {
    for (int i = buffer.count - 1; i >= 0; i--) {
        ParticleState *p = &buffer.states[i];
        p->lifetime += DeltaTime;
        p->position = p->position + p->velocity * DeltaTime;

        if (p->lifetime >= p->duration) {
            p->emitter->count--;

            if (p->emitter->count < 0) {
                int x = 5;
            }
            
            // Delete by swapping the last element into
            // the location we are deleting.
            int32 lastIndex = buffer.count - 1;
            buffer.states[i] = buffer.states[lastIndex];

            buffer.count--;
        }
    }

    // Render
    for (int i = 0; i < buffer.count; i++) {
        ParticleState *p = &buffer.states[i];
        SetTileColor(p->position, p->color);
    }
}
