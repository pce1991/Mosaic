
uint32 WidgetID(const char *name) {
    uint32 hash = 2166136261;
    while (*name) {
        hash ^= (uint8)*name++;
        hash *= 16777619;
    }
    return hash;
}

UIStyle UICopyStyle() {
    return UI->styleStack[UI->styleTop];
}

void UIPushStyle(UIStyle style) {
    if (UI->styleTop < UI_STYLE_STACK_MAX - 1) {
        UI->styleTop++;
        UI->styleStack[UI->styleTop] = style;
    }
}

void UIPopStyle() {
    if (UI->styleTop > 0) {
        UI->styleTop--;
    }
}

WidgetRect GetNextWidgetBounds() {
    WidgetRect r = {};
    r.origin = UI->cursor;
    return r;
}

static real32 MeasureTextWidth(FontTable *font, const char *str, real32 size) {
    real32 width = 0;
    while (*str) {
        int32 codepoint = *str - 32;
        if (codepoint >= 0 && codepoint < font->glyphCount) {
            width += font->glyphs[codepoint].xAdvance * size;
        }
        str++;
    }
    return width;
}

static vec2 UIScreenPos(vec2 pos, vec2 size) {
    return V2(pos.x, (real32)Game->screenHeight - pos.y);
}

void UIBegin(vec2 origin) {
    UI->cursor = origin;
    UI->columnOrigin = origin;
    UI->currentColumn = 0;
    UI->lastWidget = {};
    UI->hasPlacedWidget = false;
    UI->hoveredID = 0;
    UI->pressedID = 0;
    UI->mousePos = V2(Input->mousePos.x, (real32)Game->screenHeight - Input->mousePos.y);

    UI->windowTop = 0;

    UI->styleTop = 0;
    UIStyle *style = &UI->styleStack[0];
    style->buttonColor = V4(0.3f, 0.3f, 0.3f, 1.0f);
    style->buttonHoverColor = V4(0.4f, 0.4f, 0.4f, 1.0f);
    style->buttonActiveColor = V4(0.5f, 0.5f, 0.5f, 1.0f);
    style->font = &Game->monoFont;
    style->textSize = 32.0f;
    style->widgetSpacing = 8.0f;
    style->columnGap = 16.0f;
}

void UIPushWindow(vec2 pos, vec2 size, vec4 color, Sprite *texture) {
    if (UI->windowTop >= UI_WINDOW_STACK_MAX) return;

    UIWindowFrame *frame = &UI->windowStack[UI->windowTop++];
    frame->cursor = UI->cursor;
    frame->columnOrigin = UI->columnOrigin;
    frame->currentColumn = UI->currentColumn;
    frame->pos = pos;
    frame->size = size;

    vec2 sp = UIScreenPos(pos, size);
    DrawRectScreen(sp, size, color);
    if (texture) {
        DrawSpriteScreen(sp, size, texture);
    }

    PushClipRect(pos, size);

    UI->cursor = pos;
    UI->columnOrigin = pos;
    UI->currentColumn = 0;
}

void UIPopWindow() {
    if (UI->windowTop <= 0) return;

    UIWindowFrame *frame = &UI->windowStack[--UI->windowTop];

    UI->cursor = frame->cursor;
    UI->columnOrigin = frame->columnOrigin;
    UI->currentColumn = frame->currentColumn;

    PopClipRect();
}

bool UIButton(vec2 size, const char *label) {
    UIStyle style = UICopyStyle();
    vec2 pos = UI->cursor;

    uint32 id = WidgetID(label);

    Rect rect = {};
    rect.min = pos;
    rect.max = pos + size;

    bool hovered = PointRectTest(rect, UI->mousePos);
    bool clicked = false;

    if (hovered) {
        UI->hoveredID = id;
        if (InputPressed(Game->mouse, Input_MouseLeft)) {
            UI->pressedID = id;
            clicked = true;
        }
    }

    vec4 color = style.buttonColor;
    if (hovered) {
        color = style.buttonHoverColor;
    }
    if (clicked || (hovered && InputHeld(Game->mouse, Input_MouseLeft))) {
        color = style.buttonActiveColor;
    }

    DrawRectScreen(UIScreenPos(pos, size), size, color);

    real32 textWidth = MeasureTextWidth(style.font, label, style.textSize);
    real32 textHeight = style.font->lineHeight * style.textSize;
    vec2 textPos = V2(
        pos.x + (size.x - textWidth) * 0.5f,
        pos.y + (size.y - textHeight) * 0.5f
    );
    DrawUIText(style.font, textPos, style.textSize, V4(1), false, label);

    UI->lastWidget = { pos, size };
    UI->cursor.y = pos.y + size.y + style.widgetSpacing;
    UI->hasPlacedWidget = true;

    return clicked;
}

void UILabel(vec4 color, real32 textSize, const char *fmt, ...) {
    UIStyle style = UICopyStyle();
    vec2 pos = UI->cursor;

    char label[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(label, sizeof(label), fmt, args);
    va_end(args);

    real32 textWidth = MeasureTextWidth(style.font, label, textSize);
    real32 textHeight = style.font->lineHeight * textSize;
    vec2 widgetSize = V2(textWidth, textHeight);

    DrawUIText(style.font, pos, textSize, color, false, label);

    UI->lastWidget = { pos, widgetSize };
    UI->cursor.y = pos.y + widgetSize.y + style.widgetSpacing;
    UI->hasPlacedWidget = true;
}

void UIPushImage(vec2 size, Sprite *texture) {
    vec2 pos = UI->cursor;

    DrawSpriteScreen(UIScreenPos(pos, size), size, texture);

    UI->lastWidget = { pos, size };
    UI->cursor.y = pos.y + size.y + UICopyStyle().widgetSpacing;
    UI->hasPlacedWidget = true;
}

void UINextColumn(real32 width) {
    UIStyle style = UICopyStyle();
    UI->currentColumn++;
    UI->cursor.x = UI->columnOrigin.x + UI->currentColumn * (width + style.columnGap);
    UI->cursor.y = UI->columnOrigin.y;
    UI->lastWidget = {};
    UI->hasPlacedWidget = false;
}
