
struct WidgetRect {
    vec2 origin;
    vec2 size;
};

struct UIStyle {
    vec4 buttonColor;
    vec4 buttonHoverColor;
    vec4 buttonActiveColor;
    FontTable *font;
    real32 textSize;
    real32 widgetSpacing;
    real32 columnGap;
};

#define UI_STYLE_STACK_MAX 32

struct UIManager {
    UIStyle styleStack[UI_STYLE_STACK_MAX];
    int32 styleTop;

    vec2 cursor;
    vec2 columnOrigin;
    int32 currentColumn;

    WidgetRect lastWidget;
    bool hasPlacedWidget;

    uint32 hoveredID;
    uint32 pressedID;
};

uint32 WidgetID(const char *name);

void UIBegin(vec2 origin);
void UIWindow(vec2 pos, vec2 size, vec4 color, Sprite *texture);
bool UIButton(vec2 size, const char *label);
void UILabel(vec4 color, real32 textSize, const char *fmt, ...);
void UIPushImage(vec2 size, Sprite *texture);
void UINextColumn(real32 width);
WidgetRect GetNextWidgetBounds();

void UIPushStyle(UIStyle style);
void UIPopStyle();
UIStyle UICopyStyle();
