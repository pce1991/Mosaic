
Sprite testSprite;
int32 clickCount = 0;

void MyGameInit() {
    LoadSprite("data/textures/galaga_ship.png", &testSprite);
}

void MyGameUpdate() {
    ClearColor(RGB(0.1f, 0.1f, 0.12f));

    UIBegin(V2(50, 50));

    // -- Window 1: basic controls --
    UIPushWindow(V2(50, 50), V2(350, 420), V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);

    UILabel(V4(1.0f), 28.0f, "UI Test");
    UILabel(V4(0.6f, 0.6f, 0.6f, 1.0f), 14.0f, "Click counters and styles");

    if (UIButton(V2(200, 40), "Default Button")) {
        clickCount++;
    }

    // green style — copy current, override only what we want
    UIStyle greenStyle = UICopyStyle();
    greenStyle.buttonColor       = V4(0.15f, 0.45f, 0.15f, 1.0f);
    greenStyle.buttonHoverColor  = V4(0.2f, 0.55f, 0.2f, 1.0f);
    greenStyle.buttonActiveColor = V4(0.25f, 0.65f, 0.25f, 1.0f);
    UIPushStyle(greenStyle);
    if (UIButton(V2(200, 40), "Green Button")) {
        clickCount++;
    }
    UIPopStyle();

    // blue style
    UIStyle blueStyle = UICopyStyle();
    blueStyle.buttonColor       = V4(0.15f, 0.25f, 0.55f, 1.0f);
    blueStyle.buttonHoverColor  = V4(0.2f, 0.3f, 0.65f, 1.0f);
    blueStyle.buttonActiveColor = V4(0.25f, 0.35f, 0.75f, 1.0f);
    UIPushStyle(blueStyle);
    if (UIButton(V2(200, 40), "Blue Button")) {
        clickCount++;
    }
    UIPopStyle();

    UILabel(V4(1.0f, 0.8f, 0.2f, 1.0f), 20.0f, "Clicked: %d", clickCount);

    UIPopWindow();

    // -- Window 2: images and column demo --
    UIPushWindow(V2(430, 50), V2(300, 420), V4(0.12f, 0.15f, 0.18f, 0.95f), NULL);

    UILabel(V4(1.0f), 22.0f, "Images");

    UIPushImage(V2(64, 64), &testSprite);
    UIPushImage(V2(128, 128), &testSprite);

    UILabel(V4(0.7f, 0.7f, 0.7f, 1.0f), 14.0f, "Column layout");

    // two columns within this window
    if (UIButton(V2(120, 30), "Left")) {
        clickCount++;
    }
    UINextColumn(120);
    if (UIButton(V2(120, 30), "Right")) {
        clickCount++;
    }

    UIPopWindow();

    // -- Window 3: stacked buttons --
    UIPushWindow(V2(760, 50), V2(280, 420), V4(0.18f, 0.12f, 0.15f, 0.95f), NULL);

    UILabel(V4(1.0f), 22.0f, "Column 3");

    UIPushImage(V2(32, 32), &testSprite);
    UIPushImage(V2(32, 32), &testSprite);
    UIPushImage(V2(32, 32), &testSprite);

    if (UIButton(V2(200, 30), "Another Button")) {
        clickCount++;
    }
    if (UIButton(V2(200, 30), "One More")) {
        clickCount++;
    }

    UIPopWindow();

    // hover indicator outside any window
    if (UI->hoveredID != 0) {
        DrawTextScreenPixel(&Game->monoFont, V2(10.0f, 10.0f), 14.0f, V4(0.5f), "hovered: 0x%08x", UI->hoveredID);
    }
}
