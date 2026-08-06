

// Showcases per-tile sprites: every tile gets a sprite (loaded from a png)
// drawn on top of its colored rect. The tile color and sprite tint form a
// color gradient that drifts over time and warms up around the mouse.
// Tint, rotation, and scale only affect the sprite -- the rect underneath is
// always drawn as usual.
//
// @NOTE: sprites are reset every frame (just like tile colors), so we set the
// sprite each frame in MyMosaicUpdate.

Sprite bokehSprite = {};

void MyMosaicInit() {
    SetMosaicGridSize(32, 32);

    LoadSprite("data/textures/bokeh_paint1.png", &bokehSprite);
}

void MyMosaicUpdate() {
    vec2i mouseInt = GetMousePosition();
    vec2 mouse = V2(mouseInt);
    if (mouseInt.x < 0 || mouseInt.y < 0) {
        mouse = V2(-1000.0f);
    }

    for (int y = 0; y < Mosaic->gridHeight; y++) {
        for (int x = 0; x < Mosaic->gridWidth; x++) {
            float32 nx = x / (Mosaic->gridWidth * 1.0f);
            float32 ny = y / (Mosaic->gridHeight * 1.0f);

            // Static diagonal gradient. Wrap so it stays in [0, 360) -- HSVToRGB
            // returns grey for hues outside that range.
            float32 hue = Modf((nx + ny) * 180.0f, 360.0f);

            // How close this tile is to the mouse, in tiles.
            float32 dist = Length(V2(x, y) - mouse);
            float32 glow = 1.0f - Clamp01(dist / 8.0f);

            // @NOTE: tint multiplies the texture (texture.rgb * tint.rgb).
            // bokeh_paint1.png is greyscale, so the tint IS the color here --
            // the texture just supplies the luminance/alpha. Near the mouse we
            // brighten in the same hue (no hue lerping = no discontinuities).
            vec3 rect = HSVToRGB(V3(hue, 0.55f, 0.40f + glow * 0.35f));
            vec3 tint = HSVToRGB(V3(hue, 0.40f, 0.80f + glow * 0.20f));

            // The rect underneath is the gradient itself, so the transparent
            // parts of the paint texture read as color instead of dark grey.
            //SetTileColor(x, y, V4(rect, 1.0f));
            SetTileSprite(x, y, &bokehSprite);
            SetTileTint(x, y, V4(tint, 1.0f));

            // Keep the blob about one tile big so overlapping translucent
            // sprites don't blend together and wash out to grey.
            float32 scale = 1.24f + glow * 0.3f + 0.05f * sinf(Time * 2.0f);
            SetTileScale(x, y, scale);

            // Sprites on a higher layer draw on top, so the tiles around the
            // mouse sit above their neighbors.
            SetTileLayer(x, y, glow > 0.0f ? 1 : 0);

            // Slow spin, a bit faster near the mouse.
            float32 rotation = (Time * 0.3f * (0.5f + glow) + (x + y));
            SetTileRotation(x, y, rotation);
        }
    }

    DrawTextTop(WHITE, "pretty tiles - move the mouse");
}
