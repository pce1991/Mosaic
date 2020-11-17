
// @TODO: show an example of creating dynamic arrays of entities.

enum EntityType {
    EntityType_Base,

    EntityType_Count,
    // This gives us the number of entity types we have.
};

struct EntityHandle {
    uint32 id;
    uint16 generation;
    EntityType type;
};

struct EntityInfo {
    // the ID is implicit. 
    uint16 generation;
    EntityType type;

    // where in our buffer for the type of entity is this?
    int32 index;
};

struct EntityManager {
    DynamicArray<EntityInfo> entities;

    DynamicArray_Untyped *typeBuffers;
};


struct Entity {
    uint32 id;
    vec2 position;
};

void AllocateEntityManager(EntityManager *em) {

    em->typeBuffers = (DynamicArray_Untyped *)malloc(sizeof(DynamicArray_Untyped) * EntityType_Count);

    //for ()
    //How do we get the dynamicarrays for a particular thing? Is it a macro?
    //I guess we could just cast it?
    //This seems like an AdvancedGame topic
}

EntityInfo *GetEntityInfo(EntityManager *em, EntityHandle handle) {
    EntityInfo *e = &em->entities[handle.id];

    if (e->generation != handle.generation || e->type != handle.type) {
        return NULL;
    }

    return e;
}

void GetEntity(EntityManager *em, EntityHandle handle) {
    
}

//================================================================================
//================================================================================


struct MyData {
    Sprite sprite;
};

MyData *Data = NULL;


void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));

    Data = (MyData *)Game->myData;

    //Allocate
}

// For multi-user when we create an ID on server we must make sure that the clients create them
// in the order that they came in.
// But since we might drop packets that is actually a really big problem!
// 

void MyGameUpdate() {

}
