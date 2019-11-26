
enum EntityType {
     EntityType_Base,
     
     EntityType_Count,
};

struct EntityInfo {
    // @NOTE: a generation of zero is invalid
    uint16 generation;

    EntityType type;
    int32 index;

    // flags?
};

struct EntityHandle {
   uint32 id;
   uint16 generation;
   EntityType type;
};

inline bool operator==(EntityHandle a, EntityHandle b) {
    return a.generation == b.generation && a.id == b.id;
}

inline bool operator!=(EntityHandle a, EntityHandle b) {
    return !(a == b);
}

bool IsEntityHandleValid(EntityHandle handle) {
    if (handle.generation == 0) {
        return false;
    }
    return true;
}

struct Entity {
    uint32 id;
    
    vec2 position;
    Rect rect;
};

struct  EntityBuffer {
    EntityType type;

    void *data;
    int32 capacity;
    int32 count;

    int32 entitySize;
};

struct EntityDB{
    EntityInfo *entities;
    int32 capacity;
    int32 count;

    EntityBuffer *buffers;

    uint32 *freeList;
    uint32 freeListCount;

    EntityHandle *toDelete;
    uint32 toDeleteCount;
};

