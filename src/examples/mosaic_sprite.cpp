

// manually create a sprite and draw it at a position
// load a sprite and draw it

Sprite mySprite = {};

Sprite shipSprite = {};

void MyMosaicInit() {
    SetMosaicGridSize(32, 32);

    mySprite.width = 8;
    mySprite.height = 8;

    // @TODO: could use an arena allocator for all the sprite data. 
    // Each pixel has 4 components (red, green, blue, and alpha)
    mySprite.size = 4 * mySprite.width * mySprite.height;
    mySprite.data = (uint8 *)malloc(mySprite.size);
    memset(mySprite.data, 0, mySprite.size);

    // The origin of a sprite is the top left. 
    for (int y = 0; y < mySprite.height; y++) {
        for (int x = 0; x < mySprite.width; x++) {
            int32 pixel = (x * 4) + (y * mySprite.width * 4);

            mySprite.data[pixel + 0] = (x / (mySprite.width * 1.0f)) * 255.0f;
            mySprite.data[pixel + 1] = (y / (mySprite.height * 1.0f)) * 255.0f;
            mySprite.data[pixel + 2] = 0;
            mySprite.data[pixel + 3] = 1.0f * 255;
        }
        //break;
    }

    LoadSprite(&shipSprite, "data/galaga_ship.png");

    // should I make a nice way to access components of the sprite?
    // Like the sprite shouldn't just be this vague data, it should be typed obviously
}

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);

    // DrawSprite takes a position specifying the top left where we start drawing our sprite. 
    DrawSprite(V2(0, 4 * sinf(Time)), &mySprite);

    DrawSprite(V2(22, 18), &mySprite);

    DrawSprite(V2(16, 16), &shipSprite);
}
