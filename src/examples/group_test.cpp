
int32 saveCount = 0;
int32 settingA = 0;
int32 settingB = 0;

void MyGameInit() {
}

void MyGameUpdate() {
    ClearColor(RGB(0.1f, 0.1f, 0.12f));

    UIBegin();

    // -- Window 1: basic auto-width buttons --
    UIPushWindow(V2(100, 100), V2(500, 500), V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);

    UIStyle titleStyle = UICopyStyle();
    titleStyle.textSize = 44.0f;
    titleStyle.lineHeight = titleStyle.textSize * 1.5f;
    UIPushStyle(titleStyle);
    UILabel("Basic Groups");
    UIPopStyle();

    UIPushGroup("actions", V2(0, 0), V2(480, 200));
    if (UIButton("Save")) {
        saveCount++;
    }
    if (UIButton("Load")) {
        saveCount++;
    }
    if (UIButton("Settings")) {
        saveCount++;
    }
    UIPopGroup();

    UILabel("Saved %d times", saveCount);

    UIPopWindow();

    // -- Window 2: nested groups --
    UIPushWindow(V2(660, 100), V2(500, 700), V4(0.12f, 0.15f, 0.18f, 0.95f), NULL);

    UIStyle nestedTitle = UICopyStyle();
    nestedTitle.textSize = 44.0f;
    nestedTitle.lineHeight = nestedTitle.textSize * 1.5f;
    UIPushStyle(nestedTitle);
    UILabel("Nested Groups");
    UIPopStyle();

    UIPushGroup("outer", V2(0, 0), V2(480, 500));
    UILabel("Outer group");

    UIPushGroup("inner_top", V2(0, 0), V2(440, 200));
    UILabel("Inner group A");
    if (UIButton("Button A1")) {
        settingA++;
    }
    if (UIButton("Button A2")) {
        settingA++;
    }
    UIPopGroup();

    UIPushGroup("inner_bottom", V2(0, 0), V2(440, 150));
    UILabel("Inner group B");
    if (UIButton("Button B")) {
        settingB++;
    }
    UIPopGroup();

    UIPopGroup();

    UILabel("A: %d  B: %d", settingA, settingB);

    UIPopWindow();

    // -- Window 3: custom padding --
    UIPushWindow(V2(1220, 100), V2(500, 500), V4(0.18f, 0.12f, 0.15f, 0.95f), NULL);

    UIStyle padTitle = UICopyStyle();
    padTitle.textSize = 44.0f;
    padTitle.lineHeight = padTitle.textSize * 1.5f;
    UIPushStyle(padTitle);
    UILabel("Custom Padding");
    UIPopStyle();

    UIStyle tightStyle = UICopyStyle();
    tightStyle.padding = 2.0f;
    UIPushStyle(tightStyle);
    UIPushGroup("tight", V2(0, 0), V2(480, 180));
    if (UIButton("Tight 1")) {
    }
    if (UIButton("Tight 2")) {
    }
    if (UIButton("Tight 3")) {
    }
    UIPopGroup();
    UIPopStyle();

    UIStyle looseStyle = UICopyStyle();
    looseStyle.padding = 30.0f;
    UIPushStyle(looseStyle);
    UIPushGroup("loose", V2(0, 0), V2(480, 250));
    if (UIButton("Loose 1")) {
    }
    if (UIButton("Loose 2")) {
    }
    UIPopGroup();
    UIPopStyle();

    UIPopWindow();
}
