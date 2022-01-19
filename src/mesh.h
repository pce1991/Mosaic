
struct Mesh {
    uint32 size;
    void *data;
    
    uint32 vertCount;
    vec3 *verts;
    vec3 *normals;

    uint32 indexCount;
    uint32 *indices;

    uint32 texcoordsCount;
    vec2 *texcoords;

    int32 vertBufferID;
    int32 indexBufferID;
};

// @TODO: material, submesh which knows the start and end indices
