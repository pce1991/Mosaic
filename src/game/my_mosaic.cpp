
#if 0

void MyMosaicInit() {
}

void MyMosaicUpdate() {
  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 16; x++) {
      if (x % 2 == 0 && y % 2 != 0) { continue; }
      Core->graphics.bloomStrength = 2.0f;

      float32 r = Lerp(0.25f, 0.5f, x / 16.0f);
      float32 g = 0.2f;
      float32 b = Lerp(0.4f, 0.9f, y / 16.0f);
      SetTileColor(x, y, r, g, b);
    }
  }
}

#else

Sprite bokehSprites[2] = {};

void MyMosaicInit() {
    SetMosaicGridSize(16, 16);
    SetMosaicScreenColor(0.0, 0.0, 0.0); 

    Mosaic->drawGrid = true;
    Mosaic->gridColor = V4(0, 0, 0, 1.0f);

    LoadSprite("data/textures/circle.png", &bokehSprites[0]);
    LoadSprite("data/textures/bokeh_paint1.png", &bokehSprites[1]);

    EnableBloom();
    Core->graphics.bloomStrength = 0.3f;
    Core->graphics.bloomBlurRadius = 2.0f;
}

void MyMosaicUpdate() {
    vec2i mouseInt = GetMousePosition();
    vec2 mouse = V2(mouseInt);
    if (mouseInt.x < 0 || mouseInt.y < 0) {
        mouse = V2(-1000.0f);
    }
    if (InputPressed(Keyboard, Input_Tab)) {
      Mosaic->bloomActive = !Mosaic->bloomActive;
    }

    if (InputPressed(Keyboard, Input_Return)) {
      Mosaic->drawGrid = !Mosaic->drawGrid;
    }


    if (InputPressed(Keyboard, Input_UpArrow)) {
      Core->graphics.bloomStrength += 0.1f;
    }
    if (InputPressed(Keyboard, Input_DownArrow)) {
      Core->graphics.bloomStrength -= 0.1f;
    }

    if (InputPressed(Keyboard, Input_RightArrow)) {
      Core->graphics.bloomBlurRadius += 0.2f;
    }
    if (InputPressed(Keyboard, Input_LeftArrow)) {
      Core->graphics.bloomBlurRadius -= 0.2f;
    }

    if (InputPressed(Keyboard, Input_PageUp)) {
      Core->graphics.bloomSoftness += 0.1f;
    }
    if (InputPressed(Keyboard, Input_PageDown)) {
      Core->graphics.bloomSoftness -= 0.1f;
    }

    if (InputPressed(Keyboard, Input_Period)) {
      Core->graphics.bloomMeltRadius += 0.25f;
    }
    if (InputPressed(Keyboard, Input_Comma)) {
      Core->graphics.bloomMeltRadius -= 0.25f;
    }

    if (InputPressed(Keyboard, Input_RightBracket)) {
      Core->graphics.bloomWideMix += 0.1f;
    }
    if (InputPressed(Keyboard, Input_LeftBracket)) {
      Core->graphics.bloomWideMix -= 0.1f;
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

            SetTileColor(x, y, V4(tint, 1.0f));

            // The rect underneath is the gradient itself, so the transparent
            // parts of the paint texture read as color instead of dark grey.
            //SetTileColor(x, y, V4(rect, 1.0f));
            SetTileSprite(x, y, &bokehSprites[1]);
            SetTileTint(x, y, V4(tint, 1.0f));

            // Keep the blob about one tile big so overlapping translucent
            // sprites don't blend together and wash out to grey.
            float32 scale = 0.9f + glow * 0.3f + 0.05f * sinf(Time * 2.0f);
            SetTileScale(x, y, 0.9f);
          // SetTileScale(x, y, scale);

            // Sprites on a higher layer draw on top, so the tiles around the
            // mouse sit above their neighbors.
            //SetTileLayer(x, y, glow > 0.0f ? 1 : 0);

            // Slow spin, a bit faster near the mouse.
            float32 rotation = (Time * 0.3f * (0.5f + glow) + (x + y));
            //SetTileRotation(x, y, rotation);
        }
    }

    DrawTextTop(WHITE, "pretty tiles - move the mouse");
}
#endif
