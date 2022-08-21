

enum ShaderType {
                 ShaderType_Vertex,
                 ShaderType_Fragment,
};

struct ShaderUniform {
    char *name;
    int32 id;
};

struct Shader {
    uint32 vertSize;
    char *vertSrc;
    
    uint32 fragSize;
    char *fragSrc;
    
    int32 vertID;
    int32 fragID;
    int32 programID;

    int32 uniformCount;
    ShaderUniform *uniforms;
};

void SetShader(Shader *shader);

typedef enum {
    ColorChannel_Red,
    ColorChannel_Green,
    ColorChannel_Blue,
    ColorChannel_Alpha,
} ColorChannel;

typedef struct {
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;
} Color;


struct RectRenderData {
    vec4 color;
    mat4 model;
};

struct RectBuffer {
    int32 count;
    int32 capacity;
    RectRenderData *data;

    uint32 bufferID;
    uint32 bufferSize;
};


Color MultColor(Color c, real32 n) {
    c.r *= n;
    c.g *= n;
    c.b *= n;

    return c;
}

Color AddColor(Color c0, Color c1) {
    Color result = c0;
    result.r += c1.r;
    result.g += c1.g;
    result.b += c1.b;
    return result;
}

Color AlphaBlend_(Color src, Color dst) {
    real32 alphaT = src.a / 255.0f;
    Color c = MultColor(src, alphaT);
    Color c1 = MultColor(dst, 1 - alphaT);
    c = AddColor(c, c1);
    return c;
}


struct Sprite{
    uint32 width;
    uint32 height;
    uint32 size;
    uint8 *data; // width * height * 4

    int32 textureID;
};

struct Glyph{
    float xOffset;
    float yOffset;

    float xAdvance;

    vec2 lowerLeft;
    vec2 size;
};


struct GlyphData {
    int32 codepoint;
    vec2 position;

    vec2 dimensions;
    vec4 color;
    
};


struct FontTable {
    int32 glyphCount;
    Sprite texture;

    vec4 *texcoordsMapData;
    
    int32 texcoordsMapID;

    real32 ascent;
    real32 descent;

    real32 lineHeight;

    real32 emSize;

    Glyph *glyphs;
};


struct GlyphBuffer {
    int32 capacity;
    int32 size;
    GlyphData *data;
    int32 count;

    int32 bufferID;

    mat4 model;
    bool screen;

    FontTable *font;

    GlyphBuffer *next;
};

Glyph GetGlyph(char c, Glyph *glyphs) {
    return glyphs[c - 32];
}

// @NOTE: assumes 4 components per pixel
void AllocateSprite(Sprite *sprite, uint32 w, uint32 h) {
    sprite->width = w;
    sprite->height = h;

    sprite->size = w * h * 4;

    sprite->data = (uint8 *)malloc(sprite->size);
}

