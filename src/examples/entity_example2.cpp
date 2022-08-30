
enum EntityType {
                 EntityType_Base,
                 EntityType_Player,
                 EntityType_Enemy,
                 EntityType_Count,
};

struct EntityInfo {
    EntityType type;
    int32 indexInBuffer;
    int32 generation;
};

// Entity handle can be used to find an EntityInfo
// which can in turn be used to look up the data in buffer.
struct EntityHandle {
    EntityType type;
    int32 id;
    int32 generation;
};

struct Entity {
    //...
    vec2 position;
};

struct Player {
    Entity entity;
    real32 health;
    vec4 color;
};

struct Enemy {
    Entity entity;
    real32 health;
    int32 power;
};


struct EntityTypeBuffer {
    int32 count;
    int32 entitySizeInBytes;
    int32 capacity;

    void *entities;
};

struct EntityManager {
    int32 entityCapacity;
    int32 nextID;
    EntityInfo *entities;
    
    EntityTypeBuffer buffers[EntityType_Count];
};

void EntityManagerInit(EntityManager *em) {
    em->entityCapacity = 1000;
    em->entities = (EntityInfo *)malloc(sizeof(EntityInfo) * em->entityCapacity);
    memset(em->entities, 0, sizeof(EntityInfo) * em->entityCapacity);
    // This is important because we want to initialize the generations to 0
    
    em->nextID = 0;
    
    EntityTypeBuffer *baseBuffer = &em->buffers[EntityType_Base];

    // em->buffers[EntityType_Base].entitySizeInBytes = sizeof(Entity);
    // em->buffers[EntityType_Base].capacity = 100;
    // em->buffers[EntityType_Base].count = 0;
        
    baseBuffer->entitySizeInBytes = sizeof(Entity);
    baseBuffer->capacity = 100;
    baseBuffer->count = 0;
    baseBuffer->entities = malloc(baseBuffer->entitySizeInBytes * baseBuffer->capacity);

    EntityTypeBuffer *playerBuffer = &em->buffers[EntityType_Player];
    playerBuffer->entitySizeInBytes = sizeof(Player);
    playerBuffer->capacity = 4;
    playerBuffer->count = 0;
    playerBuffer->entities = malloc(playerBuffer->entitySizeInBytes * playerBuffer->capacity);
}

EntityHandle AddEntity(EntityManager *em, EntityType type) {
    EntityHandle h = {};

    // First thing is we need to find a new entity info
    EntityInfo *info = em->entities[em->nextID];
    info->type = type;
    
    // look up the buffer and increase the count
    // store the index in buffer for the info
    EntityTypeBuffer *buffer = &em->buffers[type];
    info->indexInBuffer = buffer->count;

    buffer->count++;
    

    // create a handle and return
    h.id = em->nextID;
    h.generation = info->generation;

    em->nextID++;

    return h;
}


// Back end function, the user wont ever call this
EntityInfo *GetEntityInfo(EntityManager *em, EntityHandle h) {
    if (h.id >= em->entityCapacity) {
        return NULL;
    }
    
    EntityInfo *info = &em->entities[h.id];

    if (info->generation != h.generation) {
        return NULL;
    }
    if (info->type != h.type) {
        return NULL;
    }

    return info;
}

void *GetEntity(EntityManager *em, EntityHandle h) {
    EntityInfo *info = GetEntityInfo(em, h);
    if (info == NULL) {
        return NULL;
    }

    // Look up the specific data in the buffer using the info
    EntityTypeBuffer *buffer = &em->buffers[info->type];

    return ((u8 *)buffer->entities + (buffer->entitySizeInBytes * info->indexInBuffer));
}

// Player *GetPlayer(EntityManager *em, EntityHandle h) {
//     return (Player *)GetEntity(em, h);
// }

void MyGameInit() {
    EntityHandle playerHandle = AddEntity(em, EntityType_Player);
    Player *p = (Player *)GetEntity(em, playerHandle);

    p->entity.position = V2(5, 5);
    p->health = 100;

    for (int i = 0; i < 100; i++) {
        EntityHandle enemyHandle = AddEntity(em, EntityType_Enemy);
        Enemy *e = (Enemy *)GetEntity(em, enemyHandle);
        e->entity.position = V2(i, i);
        e->entity.power = RandfRange(10, 50);
        e->entity.health = 10;
    }
}

void MyGameUpdate() {
    Player *p = (Player *)GetEntity(em, playerHandle);
}
