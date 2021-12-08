
// wireframe rendering of points
// lighting (toon shading first?)
// load mesh from OBJ
// generate meshes

struct MyData {
    
};

MyData *Data = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));

    Data = (MyData *)Game->myData;

    Camera *cam = &Game->camera;
    cam->type = CameraType_Perspective;
    cam->projection = Perspective(DegToRad(80.0f), 16.0f / 9.0f, 0.1f, 1000.0f);

    Game->cameraPosition = V3(0, 0, -10);
    Game->cameraRotation = IdentityQuaternion();

    mat4 camWorld = TRS(Game->cameraPosition, Game->cameraRotation, V3(1));
    cam->view = OrthogonalInverse(camWorld);
    
    cam->viewProjection = cam->projection * cam->view;
}

void MyGameUpdate() {
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    Camera *cam = &Game->camera;
    
    // Game->cameraRotation = AxisAngle(V3(1, 0, 0), sinf(Game->time) * 0.4f) * AxisAngle(V3(0, 1, 0), 0.0f);

    Game->cameraPosition = V3(0, 2, -10);
    
    mat4 camWorld = TRS(Game->cameraPosition, Game->cameraRotation, V3(1));
    cam->view = OrthogonalInverse(camWorld);
    cam->viewProjection = cam->projection * cam->view;
    
    // cam->viewProjection = cam->projection * cam->view;
    
    //DrawMesh(&Game->cube, V3(0.0f), IdentityQuaternion(), V3(1), RGB(1.0f, 0.3f, 0.3f));
    //DrawMesh(&Game->cube, V3(0.0f), AxisAngle(V3(0, 1, 0), Game->time), V3(1), RGB(1.0f, 0.3f, 0.3f));
    DrawMeshLit(&Game->cube, V3(0.0f), AxisAngle(V3(0, 1, 0), Game->time), V3(1), RGB(1.0f, 0.3f, 0.3f));
}
