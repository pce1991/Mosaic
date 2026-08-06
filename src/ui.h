#ifndef UI_H
#define UI_H

struct Sprite;
struct FontTable;

struct WidgetRect {
    vec2 origin;
    vec2 size;
};

enum UITextAlignment {
    UITextAlign_Left,
    UITextAlign_Center,
    UITextAlign_Right,
};

struct UIStyle {
    vec4 buttonColor;
    vec4 buttonHoverColor;
    vec4 buttonActiveColor;
    vec4 textColor;
    FontTable *font;
    real32 textSize;
    real32 lineHeight;
    real32 widgetSpacing;
    real32 columnGap;
    real32 padding;
    UITextAlignment textAlign;
};

#define UI_STYLE_STACK_MAX 32

struct UIWindowFrame {
    vec2 cursor;
    vec2 columnOrigin;
    int32 currentColumn;
    vec2 pos;
    vec2 size;
};

#define UI_WINDOW_STACK_MAX 32

#define UI_GROUP_STACK_MAX 32

struct UIGroupFrame {
    vec2 pos;
    vec2 size;
    uint32 id;
    vec2 cursor;
    vec2 columnOrigin;
    int32 currentColumn;
};

#define UI_WIDGET_DATA_MAX 8

struct StringFieldData {
    int32 textLength;
    int32 maxLength;
    int32 cursor;
};

union WidgetData {
    StringFieldData stringField;
};

struct WidgetDataHeader {
    uint32 id;
    int32 lastFrameAccessed;
    WidgetData *data;
};

struct UIManager {
    UIStyle styleStack[UI_STYLE_STACK_MAX];
    int32 styleTop;

    vec2 cursor;
    vec2 columnOrigin;
    int32 currentColumn;

    WidgetRect lastWidget;
    bool hasPlacedWidget;

    vec2 mousePos;

    uint32 hoveredID;
    uint32 pressedID;

    UIWindowFrame windowStack[UI_WINDOW_STACK_MAX];
    int32 windowTop;

    UIGroupFrame groupStack[UI_GROUP_STACK_MAX];
    int32 groupTop;

    uint32 activeID;

    bool widgetDataAllocated;
    BlockAllocator widgetDataAllocator;
    HashTable<uint32, WidgetDataHeader> widgetData;
};

uint32 WidgetID(const char *name);

void UIBegin();
void UIPushWindow(vec2 pos, vec2 size, vec4 color, Sprite *texture);
void UIPopWindow();
bool UIButton(real32 width, const char *label);
bool UIButton(const char *label);
void UILabel(const char *fmt, ...);
void UIPushImage(vec2 size, Sprite *texture);
void UINextColumn(real32 width);
WidgetRect GetNextWidgetBounds();

void UIPushGroup(const char *name, vec2 pos, vec2 size);
void UIPopGroup();
WidgetRect UIGroupNextBounds();

void UIPushStyle(UIStyle style);
void UIPopStyle();
UIStyle UICopyStyle();

bool UIStringField(const char *name, char *buffer, int32 bufferSize);

#endif // UI_H
