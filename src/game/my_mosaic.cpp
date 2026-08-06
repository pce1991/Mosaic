
//#include "examples\resolution.cpp"

void MyMosaicInit() {
  Mosaic->drawBorder = true; 
}

char nameInput[32] = "edit me";

void MyMosaicUpdate() {
  SetTileColor(0, 0, 0.45f, 0.3f, 0.3f);
  SetTileColor(1, 0, 0.2f, 0.6f, 0.4f);

#if 1 
  UIBegin();

  UIPushWindow(V2(140, 100), V2(700, 400), V4(0.15f, 0.15f, 0.18f, 0.95f), NULL);

  UILabel("Hello");

  if (UIButton("Hello")) {
  }

  UIStyle fieldStyle = UICopyStyle();
  fieldStyle.buttonColor       = V4(0.9f, 0.9f, 0.92f, 1.0f);
  fieldStyle.buttonHoverColor  = V4(0.94f, 0.94f, 0.97f, 1.0f);
  fieldStyle.buttonActiveColor = V4(0.75f, 0.88f, 1.0f, 1.0f);
  fieldStyle.textColor         = V4(0.1f, 0.12f, 0.16f, 1.0f);
  UIPushStyle(fieldStyle);
  if (UIStringField("name_input", nameInput, sizeof(nameInput))) {
      // Enter committed the edit
  }
  UIPopStyle();

  UIPopWindow();
#endif

}
