
void InitFont(FontTable *font, char *path) {
    int32 fontBitmapWidth = 1024;
    int32 fontBitmapHeight = 1024;

    int32 fontBitmapSize = fontBitmapWidth * fontBitmapHeight;
    uint8 *fontBitmap = (uint8 *)malloc(fontBitmapSize);

    uint32 startAscii = 32;
    uint32 endAscii = 127;
    uint32 charCount = endAscii - startAscii;

    uint32 ttfBufferSize = 1024 * 10000;
    uint8 *ttfBuffer = (uint8 *)malloc(ttfBufferSize);
    FILE *ttfFile = fopen(path, "rb");
    fread(ttfBuffer, 1, ttfBufferSize, ttfFile);
        
    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttfBuffer, 0);

    float fontPixelHeight = fontBitmapWidth / 8.0f;
    
    float scale = stbtt_ScaleForMappingEmToPixels(&info, fontPixelHeight);
    font->emSize = fontPixelHeight / scale;
    
    int32 ascent, descent;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, 0);

    font->ascent = ascent / font->emSize;
    font->descent = descent / font->emSize;

    font->lineHeight = font->ascent - font->descent;

    stbtt_bakedchar *bakedChars = (stbtt_bakedchar *)malloc(sizeof(stbtt_bakedchar) * charCount);
    
    stbtt_BakeFontBitmap(ttfBuffer, 0, fontPixelHeight, fontBitmap, fontBitmapWidth, fontBitmapHeight, startAscii, charCount, bakedChars);

    font->glyphs = (Glyph *)malloc(sizeof(Glyph) * charCount);
    Glyph *glyphs = font->glyphs;
    
    font->texcoordsMapData = (vec4 *)malloc(sizeof(vec4) * charCount);

    font->glyphCount = charCount;

    for (int i = 0; i < charCount; i++) {
        glyphs[i].xOffset = bakedChars[i].xoff / fontPixelHeight;
        glyphs[i].yOffset = bakedChars[i].yoff / fontPixelHeight;
        glyphs[i].xAdvance = bakedChars[i].xadvance / fontPixelHeight;

        stbtt_aligned_quad quad;

        real32 x = 0;
        real32 y = 0;
        stbtt_GetBakedQuad(bakedChars, fontBitmapWidth, fontBitmapHeight, i, &x, &y, &quad, 1);

        //Print("%c (%f %f) (%f %f) adv %f", i + startAscii, quad.x0, quad.y0, quad.x1, quad.y1, bakedChars[i].xadvance);

        glyphs[i].lowerLeft = V2(quad.x0 / fontPixelHeight, -quad.y1 / fontPixelHeight);

#if 1
        font->texcoordsMapData[i] = V4((bakedChars[i].x0 / (fontBitmapWidth * 1.0f)),
                                       (bakedChars[i].y0 / (fontBitmapWidth * 1.0f)),
                                       (bakedChars[i].x1 / (fontBitmapWidth * 1.0f)),
                                       (bakedChars[i].y1 / (fontBitmapWidth * 1.0f)));

        glyphs[i].size = V2(bakedChars[i].x1 - bakedChars[i].x0,
                            bakedChars[i].y1 - bakedChars[i].y0) / fontPixelHeight;
#endif
    }
    
    Sprite fontSprite;
    fontSprite.width = fontBitmapWidth;
    fontSprite.height = fontBitmapHeight;
    fontSprite.size = fontSprite.width * fontSprite.height * 4;
    fontSprite.data = (uint8 *)malloc(fontSprite.size);

    for (int i = 0; i < fontBitmapSize; i++) {
        fontSprite.data[(i * 4) + 0] = fontBitmap[i];
        fontSprite.data[(i * 4) + 1] = fontBitmap[i];
        fontSprite.data[(i * 4) + 2] = fontBitmap[i];
        fontSprite.data[(i * 4) + 3] = fontBitmap[i];
    }

    font->texture = fontSprite;

    InitFontTable(font);

    //stbi_write_png("data/font_test.png", fontBitmapWidth, fontBitmapHeight, 4, fontSprite.data, sizeof(u8) * 4 * fontBitmapWidth);
}
