
void AllocateEntityBuffer(EntityDB *db, EntityType type, uint32 size, uint32 capacity) {
    EntityBuffer *buffer = &db->buffers[type];
    
    buffer->count = 0;
    
    buffer->data = malloc(size * capacity);
    memset(buffer->data, 0, size * capacity);
    buffer->capacity = capacity;

    buffer->entitySize = size;
}

void AllocateEntityDB(EntityDB *db, uint32 capacity) {
    db->count = 0;
    
    db->entities = (EntityInfo *)malloc(capacity * sizeof(EntityInfo));
    memset(db->entities, 0, sizeof(EntityInfo) * capacity);
    db->capacity = capacity;

    db->freeList = (uint32 *)malloc(sizeof(uint32) * capacity);
    memset(db->freeList, 0, sizeof(uint32) * capacity);

    db->freeListCount = 0;

    db->toDelete = (EntityHandle *)malloc(sizeof(EntityHandle) * capacity);
    memset(db->toDelete, 0, sizeof(EntityHandle) * capacity);
    db->toDeleteCount = 0;

    db->buffers = (EntityBuffer *)malloc(sizeof(EntityBuffer) * EntityType_Count);
}

EntityHandle AddEntity(EntityDB *db, EntityType type, void *data) {
    int32 id = 0;
    EntityInfo *info = NULL;
    if (db->freeListCount > 0) {
        int32 index = db->freeList[db->freeListCount - 1];
        info = &db->entities[index];
        id = index;
    }
    else {
        id = db->count;
        info = &db->entities[db->count++];
        info->generation = 1;
    }

    info->type = type;

    EntityHandle handle = {};
    handle.id = id;
    handle.type = type;
    handle.generation = info->generation;

    EntityBuffer *buffer = &db->buffers[type];

    info->index = buffer->count;

    uint8 *dest = ((uint8 *)buffer->data + (buffer->entitySize * buffer->count));
    memcpy(dest, data, buffer->entitySize);
    buffer->count++;

    Entity *e = (Entity *)dest;
    e->id = id;

    return handle;
}


EntityInfo* GetEntityInfo(EntityDB *db, EntityHandle handle) {
    EntityInfo *e = &db->entities[handle.id];

    if (e->generation != handle.generation || e->type != handle.type) {
        return NULL;
    }

    return e;
}

void *GetEntity(EntityDB *db, EntityHandle handle) {

    if (!IsEntityHandleValid(handle)) {
        return NULL;
    }

    EntityInfo *info = GetEntityInfo(db, handle);

    if (info == NULL) { return NULL; }
    
    EntityBuffer *buffer = &db->buffers[handle.type];

    return ((uint8 *)buffer->data + (buffer->entitySize * info->index));
}

#define AccessEntity(buffer, index) ((uint8 *)buffer->data + (buffer->entitySize * index))

// @BUG: this will break if it is called on the same entity multiple times.
// Might be worth setting a flag on the entity if it's marked to be deleted
void DeleteEntity(EntityDB *db, EntityHandle handle) {
    if (GetEntityInfo(db, handle) == NULL) {
        return;
    }
    
    db->toDelete[db->toDeleteCount++] = handle;
}

void DeleteEntities(EntityDB *db) {

    for (int i = db->toDeleteCount - 1; i >= 0; i--) {

        EntityHandle handle = db->toDelete[i];
        EntityInfo *info = &db->entities[handle.id];

        EntityBuffer *buffer = &db->buffers[handle.type];
        
        if (buffer->count > 1 && (info->index < buffer->count - 1)) {
            uint32 lastIndex = buffer->count - 1;

            Entity *lastEntity = (Entity *)((uint8 *)buffer->data  + (buffer->entitySize * lastIndex));

            EntityInfo *lastInfo = &db->entities[lastEntity->id];

            lastInfo->index = info->index;
            
            db->entities[handle.id].generation++;

            memcpy((uint8 *)buffer->data + (buffer->entitySize * info->index),
                   (uint8 *)buffer->data + (buffer->entitySize * lastIndex), buffer->entitySize);

            db->freeList[db->freeListCount++] = handle.id;
        }
        else {
            db->freeList[db->freeListCount++] = handle.id;
        }

        buffer->count--;
        db->count--;
    }

    db->toDeleteCount = 0;
}
