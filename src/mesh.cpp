
void AllocateTriangle(Mesh *mesh) {
    mesh->vertCount = 3;
    
    mesh->data = (void *)malloc(sizeof(vec3) * mesh->vertCount);
    mesh->size = sizeof(vec3) * mesh->vertCount;
    
    mesh->verts = (vec3 *)mesh->data;

    mesh->verts[0] = V3(-1.0f, -1.0f, 0.0f);
    mesh->verts[1] = V3(1.0f, -1.0f, 0.0f);
    mesh->verts[2] = V3(0.0f, 1.0f, 0.0f);

    mesh->indexCount = 3;

    mesh->indices = (uint32 *)malloc(sizeof(uint32) * mesh->indexCount);

    // Counter clockwise
    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 2;
}


// @TODO: need to allocate texcoords
void AllocateQuad(Mesh *mesh) {
    mesh->vertCount = 4;
    mesh->texcoordsCount = 4;

    mesh->data = (void *)malloc((sizeof(vec3) * mesh->vertCount) +
                                (sizeof(vec2) * mesh->texcoordsCount));
    mesh->size = (sizeof(vec3) * mesh->vertCount) + (sizeof(vec2) * mesh->texcoordsCount);
    
    
    mesh->verts = (vec3 *)mesh->data;

    mesh->verts[0] = V3(-1.0f, 1.0f, 0.0f);
    mesh->verts[1] = V3(1.0f, 1.0f, 0.0f);
    mesh->verts[2] = V3(1.0f, -1.0f, 0.0f);
    mesh->verts[3] = V3(-1.0f, -1.0f, 0.0f);

#if 1
    mesh->texcoords = (vec2 *)((uint8 *)mesh->data + (sizeof(vec3) * mesh->vertCount));

    mesh->texcoords[0] = V2(0, 0);
    mesh->texcoords[1] = V2(1, 0);
    mesh->texcoords[2] = V2(1, 1);
    mesh->texcoords[3] = V2(0, 1);
#endif    

    mesh->indexCount = 6;

    mesh->indices = (uint32 *)malloc(sizeof(uint32) * mesh->indexCount);

    // Counter clockwise
    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 2;

    mesh->indices[3] = 0;
    mesh->indices[4] = 2;
    mesh->indices[5] = 3;
}


void AllocateGlyphQuad(Mesh *mesh) {
    mesh->vertCount = 4;
    mesh->texcoordsCount = 4;

    mesh->data = (void *)malloc((sizeof(vec3) * mesh->vertCount) +
                                (sizeof(vec2) * mesh->texcoordsCount));
    mesh->size = (sizeof(vec3) * mesh->vertCount) + (sizeof(vec2) * mesh->texcoordsCount);
    
    
    mesh->verts = (vec3 *)mesh->data;

    mesh->verts[0] = V3(0, 0, 0.0f);
    mesh->verts[1] = V3(1.0f, 0.0f, 0.0f);
    mesh->verts[2] = V3(1.0f, 1.0f, 0.0f);
    mesh->verts[3] = V3(0.0f, 1.0f, 0.0f);
    

#if 1
    mesh->texcoords = (vec2 *)((uint8 *)mesh->data + (sizeof(vec3) * mesh->vertCount));

    mesh->texcoords[0] = V2(0, 0);
    mesh->texcoords[1] = V2(1, 0);
    mesh->texcoords[2] = V2(1, 1);
    mesh->texcoords[3] = V2(0, 1);
#endif    

    mesh->indexCount = 6;

    mesh->indices = (uint32 *)malloc(sizeof(uint32) * mesh->indexCount);

    // Counter clockwise
    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 2;

    mesh->indices[3] = 0;
    mesh->indices[4] = 2;
    mesh->indices[5] = 3;
}
