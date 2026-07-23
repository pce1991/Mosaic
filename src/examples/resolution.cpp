
/* Font rendering demo: world, screen normalized, screen pixel */

void MyMosaicInit() {
  SetMosaicGridSize(16, 9);
}

void MyMosaicUpdate() {
  ClearTiles(V4(0.1f, 0.1f, 0.12f, 1.0f));

  // World space: size = ems, where 1 em = 1 world unit
  DrawText(&Game->monoFont, V2(-7.0f, 3.5f), 1.0f, V4(0.0f, 1.0f, 0.0f, 1.0f), "World 1.0 em");
  DrawText(&Game->monoFont, V2(-7.0f, 2.0f), 0.5f, V4(0.0f, 1.0f, 0.0f, 1.0f), "World 0.5 em");
  DrawText(&Game->monoFont, V2(-7.0f, 0.5f), 2.0f, V4(0.0f, 1.0f, 0.0f, 1.0f), "World 2.0 em");

  // Screen normalized: pos and size in [0,1], size is fraction of screen width
  DrawTextScreen(&Game->monoFont, V2(0.5f, 0.95f), 0.04f, V4(1.0f, 1.0f, 0.0f, 1.0f), true,
                 "Normalized 4%% width");
  DrawTextScreen(&Game->monoFont, V2(0.5f, 0.90f), 0.02f, V4(1.0f, 1.0f, 0.0f, 1.0f), true,
                 "Normalized 2%% width");

  // Screen pixel: pos and size in pixels
  DrawTextScreenPixel(&Game->monoFont, V2(20.0f, 30.0f), 24.0f, V4(1.0f, 0.0f, 0.0f, 1.0f), false,
                      "Pixel 24px");
  DrawTextScreenPixel(&Game->monoFont, V2(20.0f, 60.0f), 48.0f, V4(1.0f, 0.0f, 0.0f, 1.0f), false,
                      "Pixel 48px");
}
