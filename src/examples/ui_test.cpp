
Sprite testSprite;
int32 clickCount = 0;

void MyGameInit() {
    LoadSprite("data/textures/galaga_ship.png", &testSprite);
}

void MyGameUpdate() {
    ClearColor(RGB(0.1f, 0.1f, 0.12f));

    UIBegin();

    // -- Window 1: basic controls --
    UIPushWindow(V2(100, 100), V2(700, 840), V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);

    UIStyle titleStyle = UICopyStyle();
    titleStyle.textSize = 56.0f;
    titleStyle.lineHeight = titleStyle.textSize * 1.5f;
    UIPushStyle(titleStyle);
    UILabel("UI Test");
    UIPopStyle();

    UIStyle subtitleStyle = UICopyStyle();
    subtitleStyle.textColor = V4(0.6f, 0.6f, 0.6f, 1.0f);
    subtitleStyle.textSize = 28.0f;
    subtitleStyle.lineHeight = subtitleStyle.textSize * 1.5f;
    UIPushStyle(subtitleStyle);
    UILabel("Click counters and styles");
    UIPopStyle();

    UIPushGroup("buttons", V2(0, 0), V2(680, 300));
    if (UIButton("Default Button")) {
        clickCount++;
    }

    // green style
    UIStyle greenStyle = UICopyStyle();
    greenStyle.buttonColor       = V4(0.15f, 0.45f, 0.15f, 1.0f);
    greenStyle.buttonHoverColor  = V4(0.2f, 0.55f, 0.2f, 1.0f);
    greenStyle.buttonActiveColor = V4(0.25f, 0.65f, 0.25f, 1.0f);
    UIPushStyle(greenStyle);
    if (UIButton("Green Button")) {
        clickCount++;
    }
    UIPopStyle();

    // blue style
    UIStyle blueStyle = UICopyStyle();
    blueStyle.buttonColor       = V4(0.15f, 0.25f, 0.55f, 1.0f);
    blueStyle.buttonHoverColor  = V4(0.2f, 0.3f, 0.65f, 1.0f);
    blueStyle.buttonActiveColor = V4(0.25f, 0.35f, 0.75f, 1.0f);
    UIPushStyle(blueStyle);
    if (UIButton("Blue Button")) {
        clickCount++;
    }
    UIPopStyle();
    UIPopGroup();

    UIStyle countStyle = UICopyStyle();
    countStyle.textColor = V4(1.0f, 0.8f, 0.2f, 1.0f);
    countStyle.textSize = 40.0f;
    countStyle.lineHeight = countStyle.textSize * 1.5f;
    UIPushStyle(countStyle);
    UILabel("Clicked: %d", clickCount);
    UIPopStyle();

    UIPopWindow();

    // -- Window 2: images and column demo --
    UIPushWindow(V2(860, 100), V2(600, 840), V4(0.12f, 0.15f, 0.18f, 0.95f), NULL);

    UIStyle imgTitleStyle = UICopyStyle();
    imgTitleStyle.textSize = 44.0f;
    imgTitleStyle.lineHeight = imgTitleStyle.textSize * 1.5f;
    UIPushStyle(imgTitleStyle);
    UILabel("Images");
    UIPopStyle();

    UIPushImage(V2(128, 128), &testSprite);
    UIPushImage(V2(256, 256), &testSprite);

    UIStyle colLabelStyle = UICopyStyle();
    colLabelStyle.textColor = V4(0.7f, 0.7f, 0.7f, 1.0f);
    colLabelStyle.textSize = 28.0f;
    colLabelStyle.lineHeight = colLabelStyle.textSize * 1.5f;
    UIPushStyle(colLabelStyle);
    UILabel("Column layout");
    UIPopStyle();

    // two columns within this window
    if (UIButton(240, "Left")) {
        clickCount++;
    }
    UINextColumn(240);
    if (UIButton(240, "Right")) {
        clickCount++;
    }

    UIPopWindow();

    // -- Window 3: stacked buttons --
    UIPushWindow(V2(1520, 100), V2(560, 840), V4(0.18f, 0.12f, 0.15f, 0.95f), NULL);

    UIStyle col3Style = UICopyStyle();
    col3Style.textSize = 44.0f;
    col3Style.lineHeight = col3Style.textSize * 1.5f;
    UIPushStyle(col3Style);
    UILabel("Column 3");
    UIPopStyle();

    UIPushImage(V2(64, 64), &testSprite);
    UIPushImage(V2(64, 64), &testSprite);
    UIPushImage(V2(64, 64), &testSprite);

    if (UIButton(400, "Another Button")) {
        clickCount++;
    }
    if (UIButton(400, "One More")) {
        clickCount++;
    }

    UIPopWindow();

    // hover indicator outside any window
    if (UI->hoveredID != 0) {
        DrawTextScreenPixel(&Core->graphics.monoFont, V2(20.0f, 20.0f), 28.0f, V4(0.5f), "hovered: 0x%08x", UI->hoveredID);
    }
}
