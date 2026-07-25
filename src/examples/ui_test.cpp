
Sprite testSprite;
int32 clickCount = 0;
int32 logFrame = 0;

void MyGameInit() {
    LoadSprite("data/textures/galaga_ship.png", &testSprite);
}

void MyGameUpdate() {
    ClearColor(RGB(0.1f, 0.1f, 0.12f));

    UIBegin(V2(100, 100));

    // -- Window 1: basic controls --
    UIPushWindow(V2(100, 100), V2(700, 840), V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);

    UILabel(V4(1.0f), 56.0f, "UI Test");
    UILabel(V4(0.6f, 0.6f, 0.6f, 1.0f), 28.0f, "Click counters and styles");

    if (UIButton(V2(400, 80), "Default Button")) {
        clickCount++;
    }

    // green style — copy current, override only what we want
    UIStyle greenStyle = UICopyStyle();
    greenStyle.buttonColor       = V4(0.15f, 0.45f, 0.15f, 1.0f);
    greenStyle.buttonHoverColor  = V4(0.2f, 0.55f, 0.2f, 1.0f);
    greenStyle.buttonActiveColor = V4(0.25f, 0.65f, 0.25f, 1.0f);
    UIPushStyle(greenStyle);
    if (UIButton(V2(400, 80), "Green Button")) {
        clickCount++;
    }
    UIPopStyle();

    // blue style
    UIStyle blueStyle = UICopyStyle();
    blueStyle.buttonColor       = V4(0.15f, 0.25f, 0.55f, 1.0f);
    blueStyle.buttonHoverColor  = V4(0.2f, 0.3f, 0.65f, 1.0f);
    blueStyle.buttonActiveColor = V4(0.25f, 0.35f, 0.75f, 1.0f);
    UIPushStyle(blueStyle);
    if (UIButton(V2(400, 80), "Blue Button")) {
        clickCount++;
    }
    UIPopStyle();

    UILabel(V4(1.0f, 0.8f, 0.2f, 1.0f), 40.0f, "Clicked: %d", clickCount);

    UIPopWindow();

    // -- Window 2: images and column demo --
    UIPushWindow(V2(860, 100), V2(600, 840), V4(0.12f, 0.15f, 0.18f, 0.95f), NULL);

    UILabel(V4(1.0f), 44.0f, "Images");

    UIPushImage(V2(128, 128), &testSprite);
    UIPushImage(V2(256, 256), &testSprite);

    UILabel(V4(0.7f, 0.7f, 0.7f, 1.0f), 28.0f, "Column layout");

    // two columns within this window
    if (UIButton(V2(240, 60), "Left")) {
        clickCount++;
    }
    UINextColumn(240);
    if (UIButton(V2(240, 60), "Right")) {
        clickCount++;
    }

    UIPopWindow();

    // -- Window 3: stacked buttons --
    UIPushWindow(V2(1520, 100), V2(560, 840), V4(0.18f, 0.12f, 0.15f, 0.95f), NULL);

    UILabel(V4(1.0f), 44.0f, "Column 3");

    UIPushImage(V2(64, 64), &testSprite);
    UIPushImage(V2(64, 64), &testSprite);
    UIPushImage(V2(64, 64), &testSprite);

    if (UIButton(V2(400, 60), "Another Button")) {
        clickCount++;
    }
    if (UIButton(V2(400, 60), "One More")) {
        clickCount++;
    }

    UIPopWindow();

    // hover indicator outside any window
    if (UI->hoveredID != 0) {
        DrawTextScreenPixel(&Core->graphics.monoFont, V2(20.0f, 20.0f), 28.0f, V4(0.5f), "hovered: 0x%08x", UI->hoveredID);
    }

    logFrame++;
    if (logFrame == 2) {
        WriteLogToFile("ui_log.txt");
    }
}
