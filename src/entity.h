
enum EntityType {
                 EntityType_Entity,
                 EntityType_Count,
};

struct EntityHandle {
    EntityType type;
    uint32 id;
    int32 generation;
};

// every entity will need to derive from this struct.
// That way we can cast any entity to the base pointer 
struct Entity {
    EntityHandle handle;
    
    vec2 position;
};
