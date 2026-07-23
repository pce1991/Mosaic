struct Picture {
  int32 width;
  int32 height;

  vec4 *colors;
};

Picture picture = {};

vec4 ColorPalette[] = {
  V4(0.8f, 0.2f, 0.3f, 1.0f),
  V4(0.4f, 0.1f, 0.24f, 1.0f),
  V4(0.7f, 0.5f, 0.45f, 1.0f),
  V4(0.95f, 0.75f, 0.35f, 1.0f),
};

void MyMosaicInit() {
  FileHandle file;

  DynamicArray<char> characters = MakeDynamicArray<char>(&Game->frameMem, 256);

  Print("%d", (int32)t);
  
  if (OpenFileForRead("data/map_example.txt", &file, &Game->frameMem)) {
    
    while (file.offset < file.size) {
      char c = ReadChar(&file);
      PushBack(&characters, c);
    }

    int32 maxWidth = 0;
    int32 width = 0;
    int32 height = 0;
    for (int i = 0; i < characters.count; i++) {
      char c = characters[i];
      // @TODO: find out the width and height,
      // then start setting colors
      if (c == '\r' || c == ' ') {
        continue;
      }
      else if (c == '\n') {
        width = 0;
        height++;
      }
      else {
        width++;

        if (width > maxWidth) {
          maxWidth = width;
        }
      }

      Print("%d %c", characters[i], characters[i]);
    }

    Print("w: %d h: %d", maxWidth, height);

    picture.width = maxWidth;
    picture.height = height;

    picture.colors = (vec4 *)malloc(sizeof(vec4) * picture.height * picture.width);

    int32 colorIndex = 0;
    for (int i = 0; i < characters.count; i++) {
      char c = characters[i];

      if (c == 'a') {
        picture.colors[colorIndex++] = ColorPalette[0];
      }
      else if (c == 'b') {
        picture.colors[colorIndex++] = ColorPalette[1];
      }
      else if (c == 'c') {
        picture.colors[colorIndex++] = ColorPalette[2];
      }
      else if (c == 'd') {
        picture.colors[colorIndex++] = ColorPalette[3];
      }
      else {
        // do nothing
      }
    }
  }
}

void MyMosaicUpdate() {

  for (int y = 0; y < picture.height; y++) {
    for (int x = 0; x < picture.width; x++) {
      int32 index = x + (y * picture.width);
      SetTileColor(x, y, picture.colors[index]);
    }
  }
}
