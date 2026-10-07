
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

// Set this to 0 (and the #else below to #elif 1) to go back to the tiles scene.
#elif 1

static vec2 pongBallPos;
static vec2 pongBallVel;
static float32 pongLeftY;
static float32 pongRightY;
static float32 pongPrevTime;

static vec4 *pongTileColors = NULL;
static vec4 PongForeground = V4(0.55f, 0.80f, 1.00f, 1.0f);

static void PongBounceOffPaddle(float32 paddleY, float32 direction) {
    float32 speed = 45.0f;
    float32 offset = Clamp((pongBallPos.y - paddleY) / 9.0f, -1.0f, 1.0f);
    float32 angle = offset * 1.04719755f; // up to 60 degrees off horizontal
    pongBallVel.x = direction * speed * cosf(angle);
    pongBallVel.y = speed * sinf(angle);
}

Sprite bokehSprites[2] = {};

void MyMosaicInit() {
    SetMosaicGridSize(96, 54);
    SetMosaicScreenColor(0.0, 0.0, 0.0);

    LoadSprite("data/textures/flower_photo.png", &bokehSprites[0]);
    LoadSprite("data/textures/sky_paint.png", &bokehSprites[1]);

    //Mosaic->drawGrid = true;
    Mosaic->gridColor = V4(0, 0, 0, 1.0f);

    EnableBloom();
    Core->graphics.bloomStrength = 0.3f;
    Core->graphics.bloomBlurRadius = 2.0f;

    EnableBackgroundLayer();
    Mosaic->backgroundMixStrength = 0.6f;
    Mosaic->backgroundNoiseScale = 2.0f;

    // Precompute a random dark-green shade for every tile in the grid.
    pongTileColors = (vec4 *)malloc(sizeof(vec4) * Mosaic->gridWidth * Mosaic->gridHeight);
    for (int32 y = 0; y < Mosaic->gridHeight; y++) {
        for (int32 x = 0; x < Mosaic->gridWidth; x++) {
            vec3 hsv = V3(RandfRange(95.0f, 100.0f), RandfRange(0.8f, 0.95f), RandfRange(0.2f, 0.3f));
            pongTileColors[x + y * Mosaic->gridWidth] = V4(HSVToRGB(hsv), 1.0f);
        }
    }

    pongBallPos = V2(48.0f, 27.0f);
    pongBallVel.x = 45.0f * cosf(0.4f);
    pongBallVel.y = 45.0f * sinf(0.4f);
    pongLeftY = 27.0f;
    pongRightY = 27.0f;
    pongPrevTime = -1.0f;
}

void MyMosaicUpdate() {
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

    if (InputPressed(Keyboard, Input_Semicolon)) {
      Core->graphics.bloomThreshold -= 0.05f;
    }
    if (InputPressed(Keyboard, Input_Quote)) {
      Core->graphics.bloomThreshold += 0.05f;
    }

    if (InputPressed(Keyboard, Input_Minus)) {
      Core->graphics.bloomSpill -= 0.1f;
    }
    if (InputPressed(Keyboard, Input_Equal)) {
      Core->graphics.bloomSpill += 0.1f;
    }

    if (InputPressed(Keyboard, Input_B)) {
      ToggleBackgroundLayer();
    }
    if (InputPressed(Keyboard, Input_N)) {
      Mosaic->backgroundAfterBloom = !Mosaic->backgroundAfterBloom;
    }
    if (InputPressed(Keyboard, Input_T)) {
      Mosaic->backgroundMixStrength -= 0.1f;
    }
    if (InputPressed(Keyboard, Input_Y)) {
      Mosaic->backgroundMixStrength += 0.1f;
    }
    if (InputPressed(Keyboard, Input_G)) {
      Mosaic->backgroundNoiseScale -= 0.25f;
    }
    if (InputPressed(Keyboard, Input_H)) {
      Mosaic->backgroundNoiseScale += 0.25f;
    }
    if (InputPressed(Keyboard, Input_J)) {
      Mosaic->backgroundNoiseSpeed -= 0.01f;
    }
    if (InputPressed(Keyboard, Input_K)) {
      Mosaic->backgroundNoiseSpeed += 0.01f;
    }

    float32 dt = 0.0f;
    if (pongPrevTime >= 0.0f) {
        dt = Time - pongPrevTime;
    }
    pongPrevTime = Time;
    if (dt > 0.05f) { dt = 0.05f; }
    if (dt < 0.0f) { dt = 0.0f; }

    int32 width = Mosaic->gridWidth;
    int32 height = Mosaic->gridHeight;
    float32 halfPaddle = 7.5f;
    float32 ballRadius = 1.5f;

    // CPU opponents: chase the ball when it heads their way, else return to center.
    float32 leftTarget = (pongBallVel.x < 0.0f) ? pongBallPos.y : height * 0.5f;
    float32 rightTarget = (pongBallVel.x > 0.0f) ? pongBallPos.y : height * 0.5f;

    float32 leftDelta = leftTarget - pongLeftY;
    if (fabsf(leftDelta) > 1.8f) {
        float32 step = 16.5f * dt;
        pongLeftY += (leftDelta > 0.0f) ? step : -step;
        if ((leftDelta > 0.0f && pongLeftY > leftTarget) || (leftDelta < 0.0f && pongLeftY < leftTarget)) {
            pongLeftY = leftTarget;
        }
    }

    float32 rightDelta = rightTarget - pongRightY;
    if (fabsf(rightDelta) > 1.8f) {
        float32 step = 16.5f * dt;
        pongRightY += (rightDelta > 0.0f) ? step : -step;
        if ((rightDelta > 0.0f && pongRightY > rightTarget) || (rightDelta < 0.0f && pongRightY < rightTarget)) {
            pongRightY = rightTarget;
        }
    }

    pongLeftY = Clamp(pongLeftY, halfPaddle, height - halfPaddle);
    pongRightY = Clamp(pongRightY, halfPaddle, height - halfPaddle);

    // Paddle rows are shared by the collision checks and the draw below.
    int32 leftRow = (int32)floorf(pongLeftY);
    int32 rightRow = (int32)floorf(pongRightY);

    pongBallPos.x += pongBallVel.x * dt;
    pongBallPos.y += pongBallVel.y * dt;

    if (pongBallPos.y < ballRadius) {
        pongBallPos.y = ballRadius;
        pongBallVel.y = fabsf(pongBallVel.y);
    }
    if (pongBallPos.y > height - ballRadius) {
        pongBallPos.y = height - ballRadius;
        pongBallVel.y = -fabsf(pongBallVel.y);
    }

    // Paddles: 3 tiles wide at columns 4..6 and (width-7)..(width-5), 15 tall.
    if (pongBallVel.x < 0.0f && pongBallPos.x > 5.5f &&
        pongBallPos.x - ballRadius <= 7.0f &&
        pongBallPos.x + ballRadius >= 4.0f &&
        pongBallPos.y + ballRadius >= leftRow - 7.0f &&
        pongBallPos.y - ballRadius <= leftRow + 8.0f) {
        pongBallPos.x = 7.0f + ballRadius;
        PongBounceOffPaddle(leftRow + 0.5f, 1.0f);
    }

    if (pongBallVel.x > 0.0f && pongBallPos.x < width - 5.5f &&
        pongBallPos.x + ballRadius >= width - 7.0f &&
        pongBallPos.x - ballRadius <= width - 4.0f &&
        pongBallPos.y + ballRadius >= rightRow - 7.0f &&
        pongBallPos.y - ballRadius <= rightRow + 8.0f) {
        pongBallPos.x = width - 7.0f - ballRadius;
        PongBounceOffPaddle(rightRow + 0.5f, -1.0f);
    }

    // Side walls behind the paddles keep the rally alive after a miss.
    if (pongBallPos.x < ballRadius) {
        pongBallPos.x = ballRadius;
        pongBallVel.x = fabsf(pongBallVel.x);
    }
    if (pongBallPos.x > width - ballRadius) {
        pongBallPos.x = width - ballRadius;
        pongBallVel.x = -fabsf(pongBallVel.x);
    }

    // Every tile defaults to its precomputed green shade; paddles and ball overwrite it.
    for (int32 y = 0; y < height; y++) {
        for (int32 x = 0; x < width; x++) {
            SetTileColor(x, y, pongTileColors[x + y * width]);
            //SetTileTint(x, y, pongTileColors[x + y * width].rgb);
            //SetTileSprite(x, y, &bokehSprites[0]);
        }
    }

    SetBlockColor(4, leftRow - 7, 3, 15, PongForeground);
    SetBlockColor(width - 7, rightRow - 7, 3, 15, PongForeground);

    int32 ballX = (int32)floorf(pongBallPos.x);
    int32 ballY = (int32)floorf(pongBallPos.y);
    SetBlockColor(ballX - 1, ballY - 1, 3, 3, PongForeground);

    // Test background sprite: flower_photo.png stretched across a 10x10 region at
    // screen center (world origin is the center of the grid).
    DrawBackgroundSprite(V2(0.0f, 0.0f), V2(24.0f), &bokehSprites[0]);
}

#else

Sprite bokehSprites[3] = {};

void MyMosaicInit() {
    SetMosaicGridSize(32, 32);
    SetMosaicScreenColor(0.0, 0.0, 0.0); 

    Mosaic->drawGrid = true;
    Mosaic->gridColor = V4(0, 0, 0, 1.0f);

    LoadSprite("data/textures/circle.png", &bokehSprites[0]);
    LoadSprite("data/textures/sky_paint.png", &bokehSprites[1]);
    LoadSprite("data/textures/bokeh_paint1.png", &bokehSprites[2]);

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

    if (InputPressed(Keyboard, Input_Semicolon)) {
      Core->graphics.bloomThreshold -= 0.05f;
    }
    if (InputPressed(Keyboard, Input_Quote)) {
      Core->graphics.bloomThreshold += 0.05f;
    }

    if (InputPressed(Keyboard, Input_Minus)) {
      Core->graphics.bloomSpill -= 0.1f;
    }
    if (InputPressed(Keyboard, Input_Equal)) {
      Core->graphics.bloomSpill += 0.1f;
    }


int32 index = 0;

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
            //SetTileSprite(x, y, &bokehSprites[0]);
            SetTileTint(x, y, V4(tint, 1.0f));

            // Keep the blob about one tile big so overlapping translucent
            // sprites don't blend together and wash out to grey.
            float32 scale = 0.9f + glow * 0.3f + 0.05f * sinf(Time * 2.0f);
            SetTileScale(x, y, 1.25f);
            SetTileScale(x, y, scale);

            // Sprites on a higher layer draw on top, so the tiles around the
            // mouse sit above their neighbors.
            //SetTileLayer(x, y, glow > 0.0f ? 1 : 0);

            // Slow spin, a bit faster near the mouse.
            float32 rotation = (Time * 0.3f * (0.5f + glow) + (x + y));
            //SetTileRotation(x, y, rotation);
            //
            //
            if (index % 4 == 0) {
            SetTileRotation(x, y, DegToRad(90));
            }
            if (index % 3 == 0) {
            SetTileRotation(x, y, DegToRad(180));
            }
            if (index % 5 == 0) {
              SetTileRotation(x, y, DegToRad(270));
            }
            

            index++;
        }
    }

    DrawTextTop(WHITE, "pretty tiles - move the mouse");
}
#endif
