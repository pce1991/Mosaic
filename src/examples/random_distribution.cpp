
void MyMosaicInit() {
  
}

int32 const MIN = -5;
int32 const MAX = 5;
// int32 const MIN = 0;
// int32 const MAX = 2;
int32 const RANGE = MAX - MIN;
int32 total = 0;
int32 distribution[RANGE] = {};

void MyMosaicUpdate() {
  int32 r = RandiRange(MIN, MAX);

  // we remap r to be on the range (0, RANGE]
  int32 index = (r - MIN);
  distribution[index]++;
  total++;

  //Print("%d", r);

  if (InputPressed(Keyboard, Input_Space)) {
    
    for (int i = 0; i < RANGE; i++) {
      float32 rate = distribution[i] / (1.0f * total);
      Print("%d %f (%d / %d)", MIN + i, rate, distribution[i], total);
    }
  }
}
