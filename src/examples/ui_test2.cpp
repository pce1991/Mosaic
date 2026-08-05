
int32 clickCount = 0;

void MyGameInit() {
}

void MyGameUpdate() {
    ClearColor(RGB(0.1f, 0.1f, 0.12f));

    UIBegin();

    UIPushWindow(V2(100, 100), V2(700, 400), V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);

    UILabel("Hello");

    if (UIButton(400, "Hello")) {
        clickCount++;
    }

    UIPopWindow();
}
