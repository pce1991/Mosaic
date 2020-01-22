
struct Guy {
    int x;
};

struct Patrick {
    vec2 pos;

    Guy guy;

    int32 index;
    DynamicArray<EntityHandle> handles;
};


