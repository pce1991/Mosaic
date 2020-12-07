
void MyMosaicInit() {
    SetMosaicGridSize(32, 16);
}

vec2 pos = V2(4, 15);
vec2 posObstacle = V2(6, 6);

enum State {
            State_Normal,
            State_Jumping,
            State_Falling, 
};

State state = State_Normal;

real32 secondsJumpHeld = 0;

void MyMosaicUpdate() {
    ClearTiles(0, 0, 0);
    
    vec2 positionAtFrameStart = pos;

    real32 airSpeed = 3;
    real32 groundSpeed = 6;
    real32 jumpSpeed = 10;
    real32 jumpDuration = 0.5f;
    real32 gravity = 8;

    switch (state) {
        case State_Normal: {
            if (InputHeld(Input, Input_Left)) {
                pos.x -= groundSpeed * DeltaTime;
            }
            else if (InputHeld(Input, Input_Right)) {
                pos.x += groundSpeed * DeltaTime;
            }
            else {
                pos.x = GetTileCenter(pos.x);
                pos.y = GetTileCenter(pos.y);
            }

            if (InputPressed(Input, Input_Space)) {
                state = State_Jumping;
                secondsJumpHeld = 0;
            }
        } break;

        case State_Jumping: {
            if (InputReleased(Input, Input_Space)) {
                state = State_Falling;
            }
            else if (InputHeld(Input, Input_Space)) {
                pos.y -= jumpSpeed * DeltaTime;
            }

            secondsJumpHeld += DeltaTime;

            if (secondsJumpHeld >= jumpDuration) {
                state = State_Falling;
            }

            if (InputHeld(Input, Input_Left)) {
                pos.x -= airSpeed * DeltaTime;
            }
            else if (InputHeld(Input, Input_Right)) {
                pos.x += airSpeed * DeltaTime;
            }
            else {
                pos.x = GetTileCenter(pos.x);
            }
        } break;

        case State_Falling : {
            pos.y += gravity * DeltaTime;

            if (pos.y >= Mosaic->gridHeight - 1) {
                pos.y = GetTileCenter(Mosaic->gridHeight - 1);

                state = State_Normal;
            }

            if (InputHeld(Input, Input_Left)) {
                pos.x -= airSpeed * DeltaTime;
            }
            else if (InputHeld(Input, Input_Right)) {
                pos.x += airSpeed * DeltaTime;
            }
            else {
                pos.x = GetTileCenter(pos.x);
            }
        } break;
    }

    if (TilePositionsOverlap(pos, posObstacle)) {
       pos.x = GetTileCenter(positionAtFrameStart.x);
       pos.y = GetTileCenter(positionAtFrameStart.y);
    }

    SetTileColor(pos.x, pos.y, 1, 0, 0);
    SetTileColor(posObstacle.x, posObstacle.y, 0.5f, 0.5f, 0.5f);
}

// @EXERCISE: try modifying the 
