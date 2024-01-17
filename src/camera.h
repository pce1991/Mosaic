
enum CameraType : uint8 {
    CameraType_Perspective,
    CameraType_Orthographic,
};

struct Camera {
    CameraType type;

    vec3 position;
    quaternion rotation;
    
    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    // Orthographic
    real32 width;
    real32 height;

    real32 size;
};

void UpdateCamera(Camera *camera, vec3 position, quaternion rotation) {
    mat4 camWorld = TRS(position, rotation, V3(1));

    camera->projection = Orthographic(camera->width * -0.5f * camera->size, camera->width * 0.5f * camera->size,
                                      camera->height * -0.5f * camera->size, camera->height * 0.5f * camera->size,
                                      0.0, 100.0f);
    
    camera->view = OrthogonalInverse(camWorld);
    camera->viewProjection = camera->projection * camera->view;
}

void UpdateCamera(Camera *camera) {
    mat4 camWorld = TRS(camera->position, camera->rotation, V3(1));

    camera->projection = Orthographic(camera->width * -0.5f * camera->size, camera->width * 0.5f * camera->size,
                                      camera->height * -0.5f * camera->size, camera->height * 0.5f * camera->size,
                                      0.0, 100.0f);
    
    camera->view = OrthogonalInverse(camWorld);
    camera->viewProjection = camera->projection * camera->view;
}
