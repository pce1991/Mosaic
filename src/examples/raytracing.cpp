

s32 W = 256;
s32 H = 256;

Sprite texture;

struct RTCamera {
    r32 aspect;

    r32 height;
    r32 width;
    
    r32 focalLength;

    vec3 position;
    vec3 horz;
    vec3 vert;

    vec3 lowerLeft;
};

RTCamera cam;

vec4 RayColor(Ray r) {
    vec3 dir = r.direction;

    r32 t = 0.5f * (dir.y + 1.0f);

    return (1 - t) * V4(1) + t * V4(0.5f, 0.7f, 1.0f, 1.0f);
}

void SetColor(Sprite *tex, s32 x, s32 y, vec4 color) {
    // Should we do math here to flip the origin so it matches PPM in Ray Tracing in One
    // Weekend
    
    s32 pixel = (x + (y * W)) * 4;

    tex->data[pixel + 0] = color.r * 255;
    tex->data[pixel + 1] = color.g * 255;
    tex->data[pixel + 2] = color.b * 255;
    tex->data[pixel + 3] = color.a * 255;
}

void Render0() {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            r32 r = ((r32)x / (W - 1));
            r32 g = ((r32)y / (H - 1));
            r32 b = 0.25f;

            vec4 color = V4(r, g, b, 1.0f);

            SetColor(&texture, x, y, V4(r, g, b, 1.0f));
        }
    }

    OpenGL_InitTexture(&texture);
}

void Render1(RTCamera *cam) {
    vec4 color;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            r32 u = (r32)x / (W - 1);
            r32 v = (r32)y / (H - 1);

            Ray r;
            r.origin = cam->position;
            r.direction = cam->lowerLeft + (u * cam->horz) + (v * cam->vert) - cam->position;

            color = RayColor(r);
                     
            SetColor(&texture, x, y, color);
        }
    }

    OpenGL_InitTexture(&texture);
}


void MyInit() {

    {
        cam.aspect = 16.0f / 9.0f;

        cam.height = 2.0f;
        cam.width = cam.aspect * cam.height;

        cam.focalLength = 1.0f;

        cam.position = V3(0);
        cam.horz = V3(cam.width, 0, 0);
        cam.vert = V3(0, cam.height, 0);

        cam.lowerLeft = cam.position - (cam.horz * 0.5f) - (cam.vert * 0.5f) - V3(0, 0, cam.focalLength);
    }
    

    AllocateSprite(&texture, W, H);

    //Render0();

    Render1(&cam);
}


void MyGameUpdate() {

    DrawSprite(V2(0), V2(8.0f, 4.5f) * 0.75f, &texture);
}
