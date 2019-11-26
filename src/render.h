

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

#if 0
Color Lerp(Color a, Color b, real32 t) {
    return {
            Lerp(a.r, b.r, t),
            Lerp(a.g, b.g, t),
            Lerp(a.b, b.b, t),
            Lerp(a.a, b.a, t),
    };
}
#endif

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

typedef struct {
    uint16 x0;
    uint16 x1;

    uint16 y0;
    uint16 y1;

    float xOffset;
    float yOffset;

    float xAdvance;
} Glyph;


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


void DrawRect(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, int32 width, int32 height, uint8 r, uint8 g, uint8 b) {
    uint8 *row = bitmap + (startY * pitch);
    
    for (int y = 0; y < height; y++) {
        uint8 *pixel = row + (4 * startX);

        for (int x = 0; x < width; x++) {
            *pixel = b;
            pixel++;

            *pixel = g;
            pixel++;

            *pixel = r;
            pixel++;

            *pixel = 255;
            pixel++;
        }

        row += pitch;
    }
}


void DrawRect(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, int32 width, int32 height, Color color) {
    uint8 *row = bitmap + (startY * pitch);
    for (int y = 0; y < height; y++) {
        uint8 *pixel = row + (4 * startX);

        for (int x = 0; x < width; x++) {
            *pixel = color.b;
            pixel++;

            *pixel = color.g;
            pixel++;

            *pixel = color.r;
            pixel++;

            *pixel = color.a;
            pixel++;
        }

        row += pitch;
    }
}

void DrawRectAlphaBlend(uint8 *bitmap, int32 bitmapWidth, int32 bitmapHeight, uint32 pitch, int32 startX, int32 startY, int32 width, int32 height, Color color) {

    int32 clipY = 0;
    int32 clipX = 0;

    int32 newHeight = height;
    int32 newWidth = width;

    if (startY < 0) {
        clipY = 0 - startY;
        startY += clipY;
    }

    if (startX < 0) {
        clipX = 0 - startX;
        startX += clipX;
    }

    if (startY + height >= bitmapHeight) {
        clipY = (startY + height) - bitmapHeight;
    }

    if (startX + width >= bitmapWidth) {
        clipX = (startX + width) - bitmapWidth;
    }

    uint8 *row = bitmap + (startY * pitch);

    newHeight -= clipY;
    newWidth -= clipX;

    if (height <= 0) { return; }
    if (width <= 0) { return; }

    real32 alphaT = color.a / 255.0f;

    Color dst;
    Color newColor;
    
    for (int y = 0; y < newHeight; y++) {
        uint8 *pixel = row + (4 * startX);

        for (int x = 0; x < newWidth; x++) {

            dst.b = pixel[0]; dst.g = pixel[1]; dst.r = pixel[2]; dst.a = 1;

            newColor = AlphaBlend_(color, dst);
            
            *pixel = newColor.b;
            pixel++;

            *pixel = newColor.g;
            pixel++;

            *pixel = newColor.r;
            pixel++;

            *pixel = newColor.a;
            pixel++;
        }

        row += pitch;
    }
}


void DrawBox(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, int32 width, int32 height, int32 thickness, Color color) {
    uint8 *row = bitmap + (startY * pitch);
    for (int y = 0; y < height; y++) {
        uint8 *pixel = row + (4 * startX);

        for (int x = 0; x < width; x++) {

            if (y >= thickness && y < height - thickness &&
                x >= thickness && x < width - thickness) {
                pixel += 4;
                continue;
            }
            
            *pixel = color.b;
            pixel++;

            *pixel = color.g;
            pixel++;

            *pixel = color.r;
            pixel++;

            *pixel = color.a;
            pixel++;
        }

        row += pitch;
    }
}


void DrawPixel(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, Color color, int32 sizeX, int32 sizeY) {
    DrawRect(bitmap, pitch, startX, startY, sizeX, sizeY, color);
}

void DrawPixelAlphaBlend(uint8 *bitmap, int32 bitmapWidth, int32 bitmapHeight, uint32 pitch, int32 startX, int32 startY, Color color, int32 sizeX, int32 sizeY) {
    DrawRectAlphaBlend(bitmap, bitmapWidth, bitmapHeight, pitch, startX, startY, sizeX, sizeY, color);
}


void DrawSprite(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, Sprite sprite, int32 sizeX, int32 sizeY) {
    uint32 spritePitch = sprite.width * 4;
    uint8 *row = sprite.data;

    for (int y = 0; y < sprite.height; y++) {
        uint8 *pixel = row;

        uint32 yPos = startY + (y * sizeY);

        for (int x = 0; x < sprite.width; x++) {

            uint32 xPos = startX + (x * sizeX);

            Color color;
            color.r = pixel[0];
            color.g = pixel[1];
            color.b = pixel[2];
            color.a = pixel[3];

            DrawPixel(bitmap, pitch, xPos, yPos, color, sizeX, sizeY);

            pixel += 4;
        }

        row += spritePitch;
    }
}

void DrawSpriteAlphaBlend(uint8 *bitmap, int32 bitmapWidth, int32 bitmapHeight, uint32 pitch, int32 startX, int32 startY, Sprite sprite, int32 sizeX, int32 sizeY) {
    uint32 spritePitch = sprite.width * 4;
    uint8 *row = sprite.data;

    for (int y = 0; y < sprite.height; y++) {
        uint8 *pixel = row;

        uint32 yPos = startY + (y * sizeY);

        for (int x = 0; x < sprite.width; x++) {

            uint32 xPos = startX + (x * sizeX);

            Color color;
            color.r = pixel[0];
            color.g = pixel[1];
            color.b = pixel[2];
            color.a = pixel[3];

            DrawPixelAlphaBlend(bitmap, bitmapWidth, bitmapHeight, pitch, xPos, yPos, color, sizeX, sizeY);

            pixel += 4;
        }

        row += spritePitch;
    }
}


void DrawSpriteColored(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, Sprite sprite, int32 sizeX, int32 sizeY, Color color, uint8 maskChannel) {
    uint32 spritePitch = sprite.width * 4;
    uint8 *row = sprite.data;

    for (int y = 0; y < sprite.height; y++) {
        uint8 *pixel = row;

        uint32 yPos = startY + (y * sizeY);

        for (int x = 0; x < sprite.width; x++) {

            uint32 maskValue = pixel[maskChannel] > 0;

            uint32 xPos = startX + (x * sizeX);

            Color c;
            c.r = color.r * maskValue;
            c.g = color.g * maskValue;
            c.b = color.b * maskValue;
            c.a = color.a * maskValue;

            DrawPixel(bitmap, pitch, xPos, yPos, c, sizeX, sizeY);

            pixel += 4;
        }

        row += spritePitch;
    }
}

void DrawSpriteColoredAlphaBlend(uint8 *bitmap, int32 bitmapWidth, int32 bitmapHeight, uint32 pitch, int32 startX, int32 startY, Sprite sprite, int32 sizeX, int32 sizeY, Color color, uint8 maskChannel) {
    uint32 spritePitch = sprite.width * 4;
    uint8 *row = sprite.data;

    for (int y = 0; y < sprite.height; y++) {
        uint8 *pixel = row;

        uint32 yPos = startY + (y * sizeY);

        for (int x = 0; x < sprite.width; x++) {

            uint32 maskValue = pixel[maskChannel] > 0;

            uint32 xPos = startX + (x * sizeX);

            Color c;
            c.r = color.r * maskValue;
            c.g = color.g * maskValue;
            c.b = color.b * maskValue;
            c.a = color.a * maskValue;

            DrawPixelAlphaBlend(bitmap, bitmapWidth, bitmapHeight, pitch, xPos, yPos, c, sizeX, sizeY);

            pixel += 4;
        }

        row += spritePitch;
    }
}


void DrawSpriteUnscaled(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, Sprite sprite) {
    uint8 *spriteRow = sprite.data;
    uint32 spritePitch = sprite.width * 4;

    uint8 *row = bitmap + (startY * pitch);

    for (int y = 0; y < sprite.height; y++) {
        uint8 *pixel = row + (4 * startX);
        uint8 *spritePixel = spriteRow;

        for (int x = 0; x < sprite.width; x++) {
            *pixel = spritePixel[2];
            pixel++;

            *pixel = spritePixel[1];
            pixel++;

            *pixel = spritePixel[0];
            pixel++;

            *pixel = spritePixel[3];
            pixel++;

            spritePixel += 4;
        }

        row += pitch;
        spriteRow += spritePitch;
    }
}


void DrawSpriteUnscaledColored(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, Sprite sprite, Color color, uint8 maskChannel) {
    uint8 *spriteRow = sprite.data;
    uint32 spritePitch = sprite.width * 4;

    uint8 *row = bitmap + (startY * pitch);

    for (int y = 0; y < sprite.height; y++) {
        uint8 *pixel = row + (4 * startX);
        uint8 *spritePixel = spriteRow;

        for (int x = 0; x < sprite.width; x++) {
            uint8 maskValue = spritePixel[maskChannel] > 0;
            
            *pixel = color.b * maskValue;
            pixel++;

            *pixel = color.g * maskValue;
            pixel++;

            *pixel = color.r * maskValue;
            pixel++;

            *pixel = color.a * maskValue;
            pixel++;

            spritePixel += 4;
        }

        row += pitch;
        spriteRow += spritePitch;
    }
}


void DrawGlyph(uint8 *bitmap, uint32 pitch, int32 startX, int32 startY,
               Sprite fontSprite, Glyph glyph, Color color, uint32 screenHeight) {

    
    uint32 glyphHeight = glyph.y1 - glyph.y0;
    uint32 glyphWidth = glyph.x1 - glyph.x0;

    //printf("%d %d %f %f\n", glyphHeight, startY, glyph.yOffset, glyphHeight + glyph.yOffset);

    startX += glyph.xOffset;

    // @HUH? why do we need this???? Seems really bad
    startY += (screenHeight + glyph.yOffset) * 0.5f;
    
    uint8 *row = bitmap + (pitch * startY);

    uint32 spritePitch = fontSprite.width * 4;
    uint8 *spriteRow = fontSprite.data + (glyph.y0 * spritePitch);


    for (int y = 0; y < glyphHeight; y++) {

        uint8 *pixel = row + (startX * 4);
        uint8 *spritePix = spriteRow + (glyph.x0 * 4);
        
        for (int x = 0; x < glyphWidth; x++) {
            uint8 alpha = spritePix[3];

            if (alpha == 0) {
                pixel += 4;
                spritePix += 4;
                continue;
            }
            
            real32 scale = alpha / 255.0f;
            
            *pixel = color.b * scale;
            pixel++;
            spritePix++;

            *pixel = color.g * scale;
            pixel++;
            spritePix++;

            *pixel = color.r * scale;
            pixel++;
            spritePix++;

            *pixel = color.a * scale;
            pixel++;
            spritePix++;
        }

        row += pitch;
        spriteRow += spritePitch;
    }
}

void DrawString(char *str, uint32 stringCount, Sprite fontSprite, Glyph *glyphs, uint8 *bitmap, uint32 pitch, int32 startX, int32 startY, Color color, uint32 screenHeight, bool center) {

    Glyph g;

    uint32 lineCount = 1;
    for (int i = 0; i < stringCount; i++) {
        if (str[i] == '\n') {
            lineCount++;
        }
    }

    int32 *lineWidths = PushSizeFrameMem(int32, lineCount);
    memset(lineWidths, -1, sizeof(int32) * lineCount);

    Print("start %s", str);
    
    Print("lineCount %d", lineCount);
    
    int32 x_ = 0;
    int32 maxX = 0;
    
    if (center) {

        int32 lineIndex = 0;
        
        for (int i = 0; i < stringCount; i++) {
            g = GetGlyph(str[i], glyphs);

            if (str[i] == '\n' || i == stringCount - 1) {
                int32 width = maxX;
                lineWidths[lineIndex] = width;

                lineIndex++;
                maxX = 0;
                x_ = 0;
            }

            // @GACK: the glyph is just garbage if we got a newline, so dont use anything from it
            if (str[i] != '\n') {
                x_ += g.xAdvance;    
            }
            
            if (x_ > maxX) {
                maxX = x_;
            }
        }
    }

    int32 lineIndex = 0;

    int32 x = startX - (lineWidths[lineIndex] * 0.5f);
    int32 y = startY;
 
    for (int i = 0; i < stringCount; i++) {
        g = GetGlyph(str[i], glyphs);

        if (str[i] == '\n') {

            lineIndex++;

            // @HARDCODED: font size
            y += 40;
            x = startX - (lineWidths[lineIndex] * 0.5f);

            Print("width %d %d", lineIndex, lineWidths[lineIndex]);
            
            continue;
        }
        
        DrawGlyph(bitmap, pitch, x, y, fontSprite, g, color, screenHeight);
        
        x += g.xAdvance;
    }

    Print("end %s", str);
}

void BlitStretch(uint8 *bitmapA, u32 bitmapAPitch, u32 bitmapASize, u32 bitmapAWidth, u32 bitmapAHeight,
                 uint32 offsetAX, uint32 offsetAY,
                 uint8 *bitmapB, u32 bitmapBPitch, u32 bitmapBSize, u32 bitmapBWidth, u32 bitmapBHeight,
                 u32 scale) {

    uint8 *rowA = bitmapA + (bitmapAPitch * offsetAY) + (4 * offsetAX);
    uint8 *rowB = bitmapB;
    
    for (int y = 0; y < bitmapBHeight; y++) {

        uint8 *pixelA = rowA;

        for (int h = 0; h < scale; h++) {
            uint8 *pixelB = rowB;

            uint8 *pixelA = rowA;
            
            for (int x = 0; x < bitmapBWidth; x++) {

                for (int w = 0; w < scale; w++) {

                    pixelA[0] = pixelB[0];
                    pixelA[1] = pixelB[1];
                    pixelA[2] = pixelB[2];
                    pixelA[3] = pixelB[3];

                    pixelA += 4;
                }

                pixelB += 4;
            }

            rowA += bitmapAPitch;
        }

        rowB += bitmapBPitch;
    }
}

