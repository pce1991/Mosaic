
//#include "examples\resolution.cpp"

void MyMosaicInit() {
  Mosaic->drawBorder = true; 
}

void MyMosaicUpdate() {
  SetTileColor(0, 0, 0.45f, 0.3f, 0.3f);
  SetTileColor(1, 0, 0.2f, 0.6f, 0.4f);

#if 0

  UIBegin();

  UIPushWindow(V2(140, 100), V2(700, 400), V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);

  UILabel("Hello");

  if (UIButton(400, "Hello")) {
  }

  UIPopWindow();
#endif

}
