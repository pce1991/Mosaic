

struct Coin {
    vec2 position;
    vec4 color;
};

struct Player {
    vec2 position;
    vec2 velocity;
};

Player player = {};

MemoryArena arena = {};

DynamicArray<Coin> coins;


void SpawnCoin(vec2 position) {
    Coin coin = {};
    coin.position = position;
    coin.color = V4(0.7f, 0.65f, 0.2f, 1.0f);

    PushBack(&coins, coin);
}


void MyMosaicInit() {
    SetMosaicGridSize(32, 32);

    AllocateMemoryArena(&arena, Megabytes(8));
    coins = MakeDynamicArray<Coin>(&arena, 10);

    for (int i = 0; i < 10; i++) {
        // @BUG: coins could overlap in the same spot
        vec2 pos = V2(RandfRange(0, 32), RandfRange(0, 32));
        SpawnCoin(pos);
    }

    player.position = V2(16, 16);
}

void UpdatePlayer() {
    player.velocity = V2(0);

    real32 speed = 12;
    
    if (InputHeld(Keyboard, Input_RightArrow)) {
        player.velocity.x = speed;
    }
    else if (InputHeld(Keyboard, Input_LeftArrow)) {
        player.velocity.x = -speed;
    }
    else if (InputHeld(Keyboard, Input_UpArrow)) {
        player.velocity.y = -speed;
    }
    else if (InputHeld(Keyboard, Input_DownArrow)) {
        player.velocity.y = speed;
    }

    player.position = player.position + player.velocity * DeltaTime;
}

void MyMosaicUpdate() {
    //SetTileColor(0, 0, 0, 1, 0);

    if (coins.count == 0 || InputPressed(Keyboard, Input_Space)) {
        int32 randomCoinCount = RandiRange(10, 30);
        
        // METHOD 1: Random point selection with collision
        // for (int i = 0; i < randomCoinCount; i++) {
        //     // @BUG: coins could overlap in the same spot
        //     vec2 pos = V2(RandfRange(0, 32), RandfRange(0, 32));

        //     bool hitCoin = false;

        //     int32 attemptCount = 0;
        //     // keep looping until we get a position that's not occupied
        //     while (true) {
        //         for (int j = 0; j < coins.count; j++) {
        //             Coin *coin = &coins[j];

        //             if (TilePositionsOverlap(coin->position, pos)) {
        //                 //coin->color = V4(1, 0, 0, 1);
        //                 hitCoin = true;
        //             }
        //         }

        //         if (!hitCoin) {
        //             break;
        //         }
        //         else {
        //             hitCoin = false;
        //             pos = V2(RandfRange(0, 32), RandfRange(0, 32));
        //             attemptCount++;
        //         }

        //         if (attemptCount > 10) {
        //             break;
        //         }
        //     }

        //     if (!hitCoin) {
        //         SpawnCoin(pos);
        //     }
        // }

        // METHOD 2:
        // generate list of available points
        DynamicArray<vec2> openPoints = MakeDynamicArray<vec2>(&Game->frameMem, 100);
        for (int y = 0; y < Mosaic->gridHeight; y++) {
            for (int x = 0; x < Mosaic->gridWidth; x++) {
                PushBack(&openPoints, V2(x, y));
            }
        }

        // removing points that are already in use.
        for (int i = openPoints.count - 1; i >= 0; i--) {
            for (int j = 0; j < coins.count; j++) {
                Coin *coin = &coins[j];

                if (TilePositionsOverlap(coin->position, openPoints[i])) {
                    RemoveAtIndex(&openPoints, i);
                    // we break cause once we collide with a coin that's enough
                    // to remove the point from the open set, and we dont need
                    // to check for more collisions.
                    break;
                }
            }
        }

        // remove things from that list when we use a position
        for (int i = 0; i < randomCoinCount; i++) {
            if (openPoints.count == 0) {
                break;
            }
            
            // instead of generating a random position
            // we're going to generate a randomIndex from our list of open
            // positions.
            int32 index = RandiRange(0, openPoints.count);
            vec2 position = openPoints[index];

            SpawnCoin(position);

            RemoveAtIndex(&openPoints, index);
        }
    }
    
    UpdatePlayer();

    // check collisions with the coins
    //for (int i = 0; i < coins.count; i++) {

    // we iterate over the list backwards so that we guarantee when
    // removing that everything to the left our our current index
    // hasn't been touched yet in the loop. 
    for (int i = coins.count - 1; i >= 0; i--) {
        // get a pointer to the coin so we know which one it is.
        Coin *coin = &coins[i];

        if (TilePositionsOverlap(coin->position, player.position)) {
            // coin->color.r = RandfRange(0, 1);
            // coin->color.b = RandfRange(0, 1);
            RemoveAtIndex(&coins, i);
        }

        // this would create a copy of a coin from the array
        // which means anything done to "coin" here doesnt change
        // the value that's store in the array.
        //Coin coin = coins[i];
    }

    // Render
    SetTileColor(player.position, 1, 1, 1);

    for (int i = 0; i < coins.count; i++) {
        SetTileColor(coins[i].position, coins[i].color);
    }
}
