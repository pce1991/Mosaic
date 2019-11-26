
void Patrick2Update(GameMemory *gameMem) {
    Patrick *patrick = &gameMem->patrick;

    

    
    Camera *camera = &gameMem->camera;

    mat4 camWorld = TRS(gameMem->cameraPosition, gameMem->cameraRotation, V3(1));
    camera->view = OrthogonalInverse(camWorld);
    
    camera->viewProjection = camera->projection * camera->view;

    vec2 pos = V2(1.0f, 0);
    vec2 pos1 = V2(2.0f, 0.5f + 2 * sinf(Game->time * 4));
    //vec2 pos1 = V2(2.5f, 2.5f);

    real32 scale = 1;
    Rect r = {V2(-scale, -scale),
              V2(scale, scale)};

    Rect r1 = {V2(-scale, -scale),
               V2(scale, scale)};

    vec2 dir;
    bool test = RectTest(r, r1, pos, pos1, &dir);

    vec4 color = V4(1.0f, 0.2f, 0.4f, 1.0f);

    if (test) {
        color = V4(1, 0, 1, 1);
    }

    DrawRect(pos, V2(scale), color);
    DrawRect(pos1, V2(scale), V4(0, 0, 1, 1));
}
