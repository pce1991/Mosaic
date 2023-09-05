vec2 velocity;
vec2 position;

void MyMosaicInit() {
}

void MyMosaicUpdate() {
    // @TODO: create better defaults and API for this
    MosaicText *text = &Mosaic->text;
    text->cursor.x = 0.1f;
    text->cursor.y = 0.1f;

    text->color = RGB(1, 1, 1);
    text->size = 0.02f;
    
    PushText("Hellow");
    PushText("There");

    DrawTextTop(RGB(1, 0, 0), "You win!");
}
