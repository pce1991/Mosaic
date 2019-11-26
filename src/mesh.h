
struct Mesh {
    uint32 size;
    void *data;
    
    uint32 vertCount;
    vec3 *verts;

    uint32 indexCount;
    uint32 *indices;

    // @TODO: texcoords

    uint32 texcoordsCount;
    vec2 *texcoords;

    int32 vertBufferID;
    int32 indexBufferID;
};

