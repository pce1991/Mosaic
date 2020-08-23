
#define RGB(r, g, b) V4(r, g, b, 1.0f)

#define RGB8(r, g, b) V4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f)


// Usage like RGBHex(ffeeaa)
#define RGBHex(h) V4(((0x##h >> 16) & 0xff) / 255.0f,       \
                     ((0x##h >> 8) & 0xff) / 255.0f,        \
                     (0x##h & 0xff) / 255.0f,               \
                     1.0f)

// Usage like RGBH(0xffeeaa)
#define RGBH(h) V4(((h >> 16) & 0xff) / 255.0f, \
                   ((h >> 8) & 0xff) / 255.0f,  \
                   (h & 0xff) / 255.0f,         \
                   1.0f)

#define WHITE RGB(1.0f, 1.0f, 1.0f)
#define BLACK RGB(0.0f, 0.0f, 0.0f)

#define RED RGB(1.0f, 0.0f, 0.0f)
#define GREEN RGB(0.0f, 1.0f, 0.0f)
#define BLUE RGB(0.0f, 0.0f, 1.0f)

#define MAGENTA RGB(1.0f, 0.0f, 1.0f)
#define YELLOW RGB(1.0f, 1.0f, 0.0f)
#define CYAN RGB(0.0f, 1.0f, 1.0f)

#define PASTEL_RED RGBHex(fea3aa)
#define PASTEL_GREEN RGBHex(baed91)
#define PASTEL_BLUE RGBHex(b2cefe)

#define PASTEL_ORANGE RGBHex(f8b88b)
#define PASTEL_YELLOW RGBHex(faf884)
#define PASTEL_PURPLE RGBHex(f2a2e8)

    
