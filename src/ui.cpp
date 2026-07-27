
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

static vec2 MeasureTextBounds(FontTable *font, const char *str, real32 size) {
    real32 minY = 0;
    real32 maxY = 0;
    real32 x = 0;
    real32 y = 0;
    while (*str) {
        int32 codepoint = *str - 32;
        if (codepoint >= 0 && codepoint < font->glyphCount) {
            if (*str == '\n') {
                x = 0;
                y -= size * 1.5f;
            } else {
                real32 glyphMinY = y + font->glyphs[codepoint].lowerLeft.y * size;
                real32 glyphMaxY = glyphMinY + font->glyphs[codepoint].size.y * size;
                if (glyphMinY < minY) minY = glyphMinY;
                if (glyphMaxY > maxY) maxY = glyphMaxY;
                x += font->glyphs[codepoint].xAdvance * size;
            }
        }
        str++;
    }
    return V2(minY, maxY);
}

static vec2 UIScreenPos(vec2 pos, vec2 size) {
    return V2(pos.x, (real32)Core->graphics.screenHeight - pos.y);
}

void UIRectScreen(vec2 pos, vec2 size, vec4 color) {
    UICommand cmd = {};
    cmd.type = UICommand_Rect;
    cmd.pos = pos;
    cmd.size = size;
    cmd.color = color;
    PushBack(&Core->graphics.uiCommands, cmd);
}

void UISpriteScreen(vec2 pos, vec2 size, Sprite *texture) {
    UICommand cmd = {};
    cmd.type = UICommand_Sprite;
    cmd.pos = pos;
    cmd.size = size;
    cmd.texture = texture;
    PushBack(&Core->graphics.uiCommands, cmd);
}

void UIPushClipRect(vec2 pos, vec2 size) {
    UICommand cmd = {};
    cmd.type = UICommand_PushClip;
    cmd.pos = pos;
    cmd.size = size;
    PushBack(&Core->graphics.uiCommands, cmd);
}

void UIPopClipRect() {
    UICommand cmd = {};
    cmd.type = UICommand_PopClip;
    PushBack(&Core->graphics.uiCommands, cmd);
}

void UIBegin() {
    DynamicArrayClear(&Core->graphics.uiCommands);

    UI->cursor = V2(0, 0);
    UI->columnOrigin = V2(0, 0);
    UI->currentColumn = 0;
    UI->lastWidget = {};
    UI->hasPlacedWidget = false;
    UI->hoveredID = 0;
    UI->pressedID = 0;
    UI->mousePos = V2(Input->mousePos.x, (real32)Core->graphics.screenHeight - Input->mousePos.y);

    UI->windowTop = 0;

    UI->groupTop = 0;

    UI->styleTop = 0;
    UIStyle *style = &UI->styleStack[0];
    style->buttonColor = V4(0.3f, 0.3f, 0.3f, 1.0f);
    style->buttonHoverColor = V4(0.4f, 0.4f, 0.4f, 1.0f);
    style->buttonActiveColor = V4(0.5f, 0.5f, 0.5f, 1.0f);
    style->font = &Core->graphics.monoFont;
    style->textSize = 32.0f;
    style->lineHeight = style->textSize * 1.5f;
    style->textColor = V4(1.0f);
    style->textAlign = UITextAlign_Center;
    style->widgetSpacing = 8.0f;
    style->columnGap = 16.0f;
    style->padding = 10.0f;
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
    UIRectScreen(sp, size, color);
    if (texture) {
        UISpriteScreen(sp, size, texture);
    }

    UIPushClipRect(pos, size);

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

    UIPopClipRect();
}

void UIPushGroup(const char *name, vec2 pos, vec2 size) {
    if (UI->windowTop <= 0) {
        Log("UIPushGroup: no active window for group '%s'", name);
        return;
    }
    if (UI->groupTop >= UI_GROUP_STACK_MAX) {
        Log("UIPushGroup: group stack full, cannot push group '%s'", name);
        return;
    }

    UIStyle style = UICopyStyle();

    UIGroupFrame *frame = &UI->groupStack[UI->groupTop++];
    frame->id = WidgetID(name);
    frame->pos = UI->cursor + pos;
    frame->size = size;

    frame->cursor = UI->cursor;
    frame->columnOrigin = UI->columnOrigin;
    frame->currentColumn = UI->currentColumn;

    UIPushClipRect(frame->pos, size);

    vec2 origin = frame->pos + V2(style.padding, style.padding);
    UI->cursor = origin;
    UI->columnOrigin = origin;
    UI->currentColumn = 0;
}

void UIPopGroup() {
    if (UI->groupTop <= 0) return;

    UIGroupFrame *frame = &UI->groupStack[--UI->groupTop];

    UIPopClipRect();

    UI->cursor = frame->cursor;
    UI->columnOrigin = frame->columnOrigin;
    UI->currentColumn = frame->currentColumn;

    UI->cursor.y = frame->pos.y + frame->size.y + UICopyStyle().widgetSpacing;
}

WidgetRect UIGroupNextBounds() {
    WidgetRect r = {};
    r.origin = UI->cursor;
    if (UI->groupTop > 0) {
        UIGroupFrame *frame = &UI->groupStack[UI->groupTop - 1];
        UIStyle style = UICopyStyle();
        real32 paddedWidth = frame->size.x - 2.0f * style.padding;
        r.size = V2(paddedWidth, style.lineHeight);
    }
    return r;
}

bool UIButton(real32 width, const char *label) {
    UIStyle style = UICopyStyle();
    vec2 pos = UI->cursor;
    vec2 size = V2(width, style.lineHeight);

    uint32 id = WidgetID(label);

    Rect rect = {};
    rect.min = pos;
    rect.max = pos + size;

    bool hovered = PointRectTest(rect, UI->mousePos);
    bool clicked = false;

    if (hovered) {
        UI->hoveredID = id;
        if (InputPressed(Core->mouse, Input_MouseLeft)) {
            UI->pressedID = id;
            clicked = true;
        }
    }

    vec4 color = style.buttonColor;
    if (hovered) {
        color = style.buttonHoverColor;
    }
    if (clicked || (hovered && InputHeld(Core->mouse, Input_MouseLeft))) {
        color = style.buttonActiveColor;
    }

    UIRectScreen(UIScreenPos(pos, size), size, color);

    real32 textWidth = MeasureTextWidth(style.font, label, style.textSize);
    vec2 textBounds = MeasureTextBounds(style.font, label, style.textSize);
    real32 textCenterY = (textBounds.x + textBounds.y) * 0.5f;
    real32 textPosX;
    bool centerText = false;

    if (style.textAlign == UITextAlign_Left) {
        textPosX = pos.x;
    } else if (style.textAlign == UITextAlign_Right) {
        textPosX = pos.x + size.x - textWidth;
    } else {
        textPosX = pos.x + size.x * 0.5f;
        centerText = true;
    }

    vec2 textPos = V2(
        textPosX,
        pos.y + size.y * 0.5f + textCenterY - style.font->lineHeight * style.textSize
    );

    DrawUIText(style.font, textPos, style.textSize, style.textColor, centerText, label);

    UI->lastWidget = { pos, size };
    UI->cursor.y = pos.y + size.y + style.widgetSpacing;
    UI->hasPlacedWidget = true;

    return clicked;
}

bool UIButton(const char *label) {
    WidgetRect bounds = UIGroupNextBounds();
    if (bounds.size.x <= 0) return false;
    return UIButton(bounds.size.x, label);
}

void UILabel(const char *fmt, ...) {
    UIStyle style = UICopyStyle();
    vec2 pos = UI->cursor;

    char label[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(label, sizeof(label), fmt, args);
    va_end(args);

    real32 textWidth = MeasureTextWidth(style.font, label, style.textSize);
    vec2 widgetSize = V2(textWidth, style.lineHeight);

    DrawUIText(style.font, pos, style.textSize, style.textColor, false, label);

    UI->lastWidget = { pos, widgetSize };
    UI->cursor.y = pos.y + widgetSize.y + style.widgetSpacing;
    UI->hasPlacedWidget = true;
}

void UIPushImage(vec2 size, Sprite *texture) {
    vec2 pos = UI->cursor;

    UISpriteScreen(UIScreenPos(pos, size), size, texture);

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

