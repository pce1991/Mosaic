
// Generic entity system. The entity type enum is provided by the game (e.g. soccer_entity.h).
enum EntityType : int;

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

    // Smoothed position used for rendering (physics interpolates this).
    vec2 renderPosition;
};
