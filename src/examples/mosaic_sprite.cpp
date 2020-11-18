

// manually create a sprite and draw it at a position
// load a sprite and draw it

Sprite mySprite = {};

void MyMosaicInit() {
    SetMosaicGridSize(64, 64);

    mySprite.width = 5;
    mySprite.height = 5;

    // why is this real32 instead of vec4??? because we're using 256 bits per channel?
    mySprite.size = 4 * mySprite.width * mySprite.height;
    mySprite.data = (uint8 *)malloc(mySprite.size);

    // should I make a nice way to access components of the sprite?
    // Like the sprite shouldn't just be this vague data, it should be typed obviously
}

void MyMosaicUpdate() {
    
}
