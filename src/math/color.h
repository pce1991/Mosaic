
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


vec3 RGBToHSV(vec3 rgb) {
    int32 minComponent = -1;
    int32 maxComponent = -1;

    if (rgb[0] >= rgb[1] && rgb[0] >= rgb[2]) {
        maxComponent = 0;
    }
    else if (rgb[1] >= rgb[0] && rgb[1] >= rgb[2]) {
        maxComponent = 1;
    }
    else if (rgb[2] >= rgb[0] && rgb[2] >= rgb[1]) {
        maxComponent = 2;
    }

    if (rgb[0] <= rgb[1] && rgb[0] <= rgb[2]) {
        minComponent = 0;
    }
    else if (rgb[1] <= rgb[0] && rgb[1] <= rgb[2]) {
        minComponent = 1;
    }
    else if (rgb[2] <= rgb[0] && rgb[2] <= rgb[1]) {
        minComponent = 2;
    }

    ASSERT(maxComponent != -1 && minComponent != -1);
    
    real32 chroma = rgb[maxComponent] - rgb[minComponent];
    real32 hue = 0.0f;

    if (chroma != 0.0f) {
        if (maxComponent == 0) {
            hue = 60.0f * fmodf(6.0f + (rgb.g - rgb.b) / chroma, 6.0f);
        }
        else if (maxComponent == 1) {
            hue = 60.0f * ((rgb.b - rgb.r) / chroma + 2.0f);
        }
        else if (maxComponent == 2) {
            hue = 60.0f * ((rgb.r - rgb.g) / chroma + 4.0f);
        }
    }

    real32 value = rgb[maxComponent];
    real32 saturation = SafeRatio(chroma, value, 0.0f);

    vec3 hsv = V3(hue, saturation, value);
    return hsv;
}

// H: [0, 360], S, V: [0, 1]
vec3 HSVToRGB(vec3 hsv) {
    real32 chroma = hsv.v * hsv.s;
    real32 hPrime = hsv.h / 60.0f;
    real32 x = chroma * (1.0f - Absf(fmodf(hPrime, 2.0f) - 1.0f));

    vec3 rgb = {};

    if (0.0f <= hPrime && hPrime < 1.0f) {
        rgb = V3(chroma, x, 0.0f);
    }
    else if (1.0f <= hPrime && hPrime < 2.0f) {
        rgb = V3(x, chroma, 0.0f);
    }
    else if (2.0f <= hPrime && hPrime < 3.0f) {
        rgb = V3(0.0f, chroma, x);
    }
    else if (3.0f <= hPrime && hPrime < 4.0f) {
        rgb = V3(0.0f, x, chroma);
    }
    else if (4.0f <= hPrime && hPrime < 5.0f) {
        rgb = V3(x, 0.0f, chroma);
    }
    else if (5.0f <= hPrime && hPrime < 6.0f) {
        rgb = V3(chroma, 0.0f, x);
    }

    real32 min = hsv.v - chroma;
    rgb = rgb + V3(min, min, min);

    return rgb;
}
