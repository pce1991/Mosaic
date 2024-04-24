
int32 EntitySizeTable[] = {
                           sizeof(Entity),
                           0,
};

struct EntityTypeStorage {
    EntityType type;
    int32 elementSize;

    DynamicArray_Untyped array;
};

enum EntityFlag {
                 EntityFlag_None = 0,
                 EntityFlag_Deleted = 1 << 2,
};

struct EntityInfo {
    EntityType type;
    int32 generation;

    int32 index;

    EntityFlag flags;
};

struct EntityManager {
    DynamicArray<EntityInfo> entityInfo;
    DynamicArray<EntityHandle> entitiesToDelete;
    DynamicArray<uint32> freelist;

    EntityTypeStorage entities[EntityType_Count];    
};

void AllocateEntityManager(EntityManager *manager, MemoryArena *arena, int32 entityCount, int32 *entityTypeCounts) {
    uint32 entitiesPerType = 1;

    manager->entityInfo = MakeDynamicArray<EntityInfo>(arena, entityCount);
    manager->freelist = MakeDynamicArray<uint32>(arena, entityCount);
    manager->entitiesToDelete = MakeDynamicArray<EntityHandle>(arena, entityCount);

    for (int i = 0; i < EntityType_Count; i++) {
        EntityTypeStorage *storage = &manager->entities[i];
        *storage = {};
        storage->type = (EntityType)i;
        storage->elementSize = EntitySizeTable[i];

        int32 capacity = 1;
        if (entityTypeCounts != NULL) {
            capacity = Max(8, entityTypeCounts[i]);
        }

        storage->array = MakeDynamicArray(arena, storage->elementSize, capacity, 1);
    }
}


void *AddEntityToStorage(EntityManager *manager, EntityInfo *info) {
    EntityTypeStorage *storage = &manager->entities[info->type];

    void *dest = PushBackPtr(&storage->array, storage->elementSize);
    
    info->index = storage->array.count - 1;

    return dest;
}

void *AccessEntity_(EntityTypeStorage const *storage, uint32 index) {
    return DynamicArrayGetData(&storage->array, storage->elementSize, index);
}

EntityInfo *GetEntityInfo(EntityManager const *manager, EntityHandle handle) {
    if (handle.id >= manager->entityInfo.count) { return NULL; }

    EntityInfo *info = &manager->entityInfo[handle.id];

    if (info->generation != handle.generation) { return NULL; }
    if (info->type != handle.type) { return NULL; }

    return info;
}

void RemoveEntityFromStorage(EntityManager *manager, EntityInfo *deletedInfo) {
    EntityTypeStorage *storage = &manager->entities[deletedInfo->type];

    if (storage->array.count == 1) {
        storage->array.count = 0;
    }
    else {
        Entity *toSwap = (Entity *)AccessEntity_(storage, storage->array.count - 1);
        EntityInfo *infoToSwap = GetEntityInfo(manager, toSwap->handle);
        ASSERT(infoToSwap != NULL);

        void *swapDestEntity = DynamicArrayGetData(&storage->array, storage->elementSize, deletedInfo->index);
        memcpy(swapDestEntity, toSwap, storage->elementSize);

        storage->array.count--;

        infoToSwap->index = deletedInfo->index;
    }
}

EntityHandle AddEntity_(EntityManager *manager, EntityType type, void *data) {
    EntityInfo *info = NULL;
    EntityHandle handle = {};

    if (manager->freelist.count > 0) {
        PopBack(&manager->freelist, &handle.id);
        info = &manager->entityInfo[handle.id];
        uint32 generation = info->generation;
        *info = {};
        info->generation = info->generation;
    }
    else {
        handle.id = manager->entityInfo.count;
        info = PushBackPtr(&manager->entityInfo);
        info->generation = 1;
    }

    info->type = type;
    handle.generation = info->generation;
    handle.type = type;

    Entity *destEntity = (Entity *)AddEntityToStorage(manager, info);
    memcpy(destEntity, data, EntitySizeTable[type]);

    destEntity->handle = handle;

    return handle;
}

void DeleteEntity(EntityManager *manager, EntityHandle handle) {
    EntityInfo *info = GetEntityInfo(manager, handle);

    if (HasFlag(info->flags, EntityFlag_Deleted)) { return; }

    PushBack(&manager->entitiesToDelete, handle);
}

void DeleteEntities(EntityManager *manager) {
    for (int i = 0; i < manager->entitiesToDelete.count; i++) {
        EntityHandle handle = manager->entitiesToDelete[i];
        EntityInfo *info = GetEntityInfo(manager, handle);
        RemoveEntityFromStorage(manager, info);

        info->generation++;
        info->flags = (EntityFlag)AddFlag(info->flags, EntityFlag_Deleted);

        PushBack(&manager->freelist, handle.id);
   }

    DynamicArrayClear(&manager->entitiesToDelete);
}

Entity *GetEntity_(EntityManager *manager, EntityHandle handle) {
    EntityInfo *info = GetEntityInfo(manager, handle);
    if (info == NULL) { return NULL; }

    EntityTypeStorage *storage = &manager->entities[handle.type];
    if (info->index >= storage->array.count) { return NULL; }

    return (Entity *)DynamicArrayGetData(&storage->array, EntitySizeTable[handle.type], info->index);
}

#define GetEntity(em, type_name, handle) (type_name *)GetEntity_(em, handle)
#define AddEntity(em, type_name, data) AddEntity_(em, EntityType_ ## type_name, data)
#define AccessEntity(storage, type_name, index) (type_name *)AccessEntity_(storage, index)
