
void PatrickInit(GameMemory *gameMem) {
    Patrick *patrick = &gameMem->patrick;

    patrick->pos = V2(3, 3);

    AllocateEntityDB(&gameMem->entityDB, 1024);
    AllocateEntityBuffer(&gameMem->entityDB, EntityType_Base, sizeof(Entity), 256);

    for (int i = 0; i < 20; i++) {
        Entity e = {};
        e.position = V2(-4 + (i * 0.5f), sinf(i * 0.5f));
        EntityHandle h = AddEntity(&Game->entityDB, EntityType_Base, &e);

        PushBack(&patrick->handles, h);
    }
}

void PatrickUpdate(GameMemory *gameMem) {
    Patrick *patrick = &gameMem->patrick;
    InputQueue *input = &gameMem->inputQueue;
    
    Camera *camera = &gameMem->camera;

    mat4 camWorld = TRS(gameMem->cameraPosition, gameMem->cameraRotation, V3(1));
    camera->view = OrthogonalInverse(camWorld);
    
    camera->viewProjection = camera->projection * camera->view;

    vec2 pos = patrick->pos;
    vec2 pos1 = V2(0.5f, 0.5f + 2 * sinf(Game->time));
    //vec2 pos1 = V2(2.5f, 2.5f);

    real32 scale = 1;
    Rect r = {V2(-scale, -scale),
              V2(scale, scale)};

    Rect r1 = {V2(-scale, -scale),
               V2(scale, scale)};

    vec2 dir;
    bool test = RectTest(r, r1, pos, pos1, &dir);

    vec4 color = V4(0, 1, 0, 1);

    if (test) {
        color = V4(1, 0, 0, 1);
    }

    DrawRect(pos, V2(scale), color);
    DrawRect(pos1, V2(scale), V4(0, 0, 1, 1));

    vec2 shipPos = V2(0, 0);
    vec2 shipScale = V2(1, 1);
    //DrawSprite(shipPos, shipScale, &Game->galagaShip);

    vec2 testPos = V2(-2, 0);
    vec2 testScale = V2(1);
    //DrawSprite(testPos, testScale, &Game->pixelTest);

    EntityDB *db = &Game->entityDB;

    if (InputPressed(input, Input_Space)) {
        if (patrick->handles.count > 0) {
            DeleteEntity(db, patrick->handles[patrick->index]);
            RemoveAtIndex(&patrick->handles, patrick->index);
        }
    }
    
    EntityBuffer *buffer = &db->buffers[EntityType_Base];

    for (int i = 0; i < patrick->handles.count; i++) {
        EntityHandle h = patrick->handles[i];
        Entity *e = (Entity *)GetEntity(db, h);

        if (e == NULL) { continue; }

        DrawSprite(e->position, V2(0.1f), &Game->galagaShip);
    }
}
