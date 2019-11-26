
enum CameraType : uint8 {
    CameraType_Perspective,
    CameraType_Orthographic,
};

struct Camera {
    CameraType type;
    
    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    // Orthographic
    real32 width;
    real32 height;
};
