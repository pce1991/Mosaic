

struct Particle {
    vec2 position;
    vec2 velocity;
    vec4 color;
    vec4 startColor;
    vec4 endColor;
    real32 lifetime;
    real32 duration;
};

struct MyData {
    real32 lastSpawnTime;
    int32 maxParticles;
    DynamicArray<Particle> particles;
};

MyData *Data = NULL;

void MyMosaicInit() {
    Data = (MyData *)malloc(sizeof(MyData));
    memset(Data, 0, sizeof(MyData));

    Data->maxParticles = 256;
    
    SetMosaicGridSize(64, 64);
}

void MyMosaicUpdate() {

    ClearTiles(0, 0, 0);
    
    real32 timeSinceSpawn = Game->time - Data->lastSpawnTime;

    if (timeSinceSpawn > 1.0f) {
        int32 spawnCount = 8;

        if (Data->particles.count + 8 > Data->maxParticles) {
            spawnCount = Data->maxParticles - Data->particles.count;
        }

        for (int i = 0; i < spawnCount; i++) {
            Particle p = {};
            p.position = V2(32, 60);
            p.velocity = V2(RandfRange(-1, 1), -1) * 8;
            // @TODO: random direction (normalized)
            p.color = RGB(0.4f, 0.4f, 1.0f);
            p.lifetime = 0.0f;

            p.duration = RandfRange(4.0f, 8.0f);

            PushBack(&Data->particles, p);
        }
    }

    for (int i = Data->particles.count - 1; i >= 0; i--) {
        Particle *p = &Data->particles[i];

        p->position = p->position + p->velocity * Game->deltaTime;

        p->color.r += 0.1f * Game->deltaTime;
        p->color.g -= 0.3f * Game->deltaTime;
        p->color.b -= 0.3f * Game->deltaTime;
        p->color.a -= 0.1f * Game->deltaTime;

        p->lifetime += Game->deltaTime;

        if (p->lifetime >= p->duration) {
            RemoveAtIndex(&Data->particles, i);
        }
    }

    for (int i = Data->particles.count - 1; i >= 0; i--) {
        Particle *p = &Data->particles[i];

        Tile *t = GetTile(p->position);
        if (t) {
            t->color = p->color;
        }
    }
}
