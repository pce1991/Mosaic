
// @TODO: standardize if our position is the top left or the center
// center is easier visually, but is kinda quirky when it comes to bounding boxes

// @DESIGN: ball passes thru you unless you type the words which activates collision

// @TODO: if you input things incorrectly then we reset

//@TODO: definite piece position as offset on axel so that they're always consistent and we don't
// get the jitter on some pieces

// @TODO: what if game was about keeping the ball from reaching either side rather than scoring a goal? 

const int32 InputLength = 8;

struct Player {
    int32 inputIndex;
    char input[InputLength];
};

const int32 PieceSize = 5;
real32 CollisionDuration = 0.5f;

struct Piece {
    vec2 position;

    bool collisionActive;
    real32 timeCollisionActivated;
};

// @TOOD: get the pieces

const int32 GoalHalfHeight = 8;
const int32 GoalHalfWidth = 2;

struct Goal {
    vec2 center;
};

const int32 BallSize = 3;

struct Ball {
    vec2 position;
    vec2 velocity;
};

struct Axel {
    char string[InputLength];

    int32 x;
    vec2 velocity;
    
    int32 pieceCount;
    Piece *pieces;
};

const int32 AxelCount = 3;

struct Team {
    vec4 color;
    Axel axels[AxelCount];
};

struct GameMem {
    Player player;
    
    Team teams[2];
    Goal goals[2];

    Ball ball;
};

const int32 ArenaWidth = 128;
const int32 ArenaHeight = 80;

struct GameMem Foose = {};

 
void GenerateAxelString(char *string) {
    string[0] = 'a';
    string[1] = 'b';
    string[2] = 'c';
}


void StartRound() {
    Ball *ball = &Foose.ball;

    ball->position = V2(ArenaWidth / 2, ArenaHeight / 2);
    
    real32 speed = 20;
    ball->velocity = Rotate(DegToRad(RandfRange(-30, 30)), V2(-1, 0)) * speed;

    for (int t = 0; t < 2; t++) {
        Team *team = &Foose.teams[t];

        // find which direction we should move to have one of our pieces intercept the ball
        for (int a = 0; a < AxelCount; a++) {
            Axel *axel = &team->axels[a];

            GenerateAxelString(axel->string);
        }
    }
}


void MyMosaicInit() {
    //Foose.teams[1].color = V4(0.3f, 0.3f, 0.8f, 1.0f);
    
    SetMosaicGridSize(ArenaWidth, ArenaHeight);

    vec2 center = V2(ArenaWidth / 2, ArenaHeight / 2);

    int32 AxelColumns[] = {
                           // offset 10 from the center
                           54,
                           34,
                           14,

                           74,
                           94,
                           104,
    };
    
    {
        Team *team = &Foose.teams[0];
        *team = {};
        
        team->color = V4(0.7f, 0.2f, 0.0f, 1.0f);

        {
            Axel *axel = &team->axels[0];
            axel->pieceCount = 3;
            axel->pieces = (Piece*)malloc(sizeof(Piece) * axel->pieceCount);

            axel->x = AxelColumns[0];

            int32 offsetY = 20;
            int32 trimmedHeight = ArenaHeight - offsetY;
            int32 strideY = trimmedHeight / 3;
            
            vec2 cursor = V2(AxelColumns[0], offsetY);
            for (int i = 0; i < 3; i++) {
                axel->pieces[i].position = cursor;

                cursor.y += strideY;
            }
        }

        {
            Axel *axel = &team->axels[1];
            axel->pieceCount = 2;
            axel->pieces = (Piece*)malloc(sizeof(Piece) * axel->pieceCount);

            axel->x = AxelColumns[1];

            int32 offsetY = 20;
            int32 trimmedHeight = ArenaHeight - offsetY;
            int32 strideY = trimmedHeight / 2;

            // maybe better to say what the stride is, center them, then find offset from that
            
            vec2 cursor = V2(AxelColumns[1], offsetY);
            for (int i = 0; i < 3; i++) {
                axel->pieces[i].position = cursor;

                cursor.y += strideY;
            }
        }

        {
            Axel *axel = &team->axels[2];
            axel->pieceCount = 1;
            axel->pieces = (Piece*)malloc(sizeof(Piece) * axel->pieceCount);

            axel->x = AxelColumns[2];

            axel->pieces[0].position = V2(AxelColumns[2], ArenaHeight / 2);
        }
    }

    StartRound();
}

void DrawPiece(Piece *piece, vec4 color) {
    for (int y = 0; y < PieceSize; y++) {
        for (int x = 0; x < PieceSize; x++) {
            SetTileColor(piece->position.x + x, piece->position.y + y, color);
        }
    }
}

void DrawBall(Ball *ball) {
    for (int y = 0; y < BallSize; y++) {
        for (int x = 0; x < BallSize; x++) {
            SetTileColor(ball->position.x + x, ball->position.y + y, V4(0.8f, 0.8f, 0.8f, 1.0f));
        }
    }
}

void DrawGoal(Goal *goal) {
    for (int y = 0; y < GoalHalfHeight; y++) {
        for (int x = 0; x < GoalHalfWidth; x++) {
            SetTileColor(goal->center.x + x, goal->center.y + y, V4(0.3f, 0.3f, 0.3f, 1.0f));
        }
    }
}

void RenderTeam(Team *team) {
    for (int a = 0; a < AxelCount; a++) {
        Axel *axel = &team->axels[a];

        vec2 textPos = V2(axel->x, 0);
        DrawTextTile(textPos, 2.0f, V4(1), axel->string);

        for (int j = 0; j < axel->pieceCount; j++) {
            Piece *piece = &axel->pieces[j];
            vec4 color = team->color;

            if (piece->collisionActive) {
                color = V4(1, 0, 0, 1);
            }
            
            DrawPiece(&axel->pieces[j], color);
        }
    }
}

void MyMosaicUpdate() {
    ClearTiles(0.1f, 0.3f, 0.15f);

    {
        Player *player = &Foose.player;

        // @TODO: if you get a match we should also clear it
        
        if (InputPressed(Keyboard, Input_Return)) {
            for (int i = 0; i < InputLength; i++) {
                player->input[i] = 0;
            }
            
            player->inputIndex = 0;
        }
        else {
            if (Input->charCount > 0) {
                char input = Input->inputChars[0];

                if (player->inputIndex < InputLength) {
                    player->input[player->inputIndex] = input;
                    player->inputIndex++;
                }
            }
        }
    }


    {
        Player *player = &Foose.player;
        Ball *ball = &Foose.ball;
        for (int t = 0; t < 2; t++) {
            Team *team = &Foose.teams[t];

            bool foundMatch = false;
            
            // find which direction we should move to have one of our pieces intercept the ball
            for (int a = 0; a < AxelCount; a++) {
                Axel *axel = &team->axels[a];

                // activate collision
                {
                    int32 axelStringLength = 0;

                    for (int32 c = 0; c < InputLength; c++) {
                        if (axel->string[c] != 0) {
                            axelStringLength++;
                        }
                    }

                    bool matches = axelStringLength == player->inputIndex;
                                        
                    for (int32 c = 0; c < player->inputIndex; c++) {
                        if (axel->string[c] != player->input[c]) {
                            matches = false;
                        }
                    }

                    if (matches) {
                        for (int32 i = 0; i < axel->pieceCount; i++) {
                            Piece *piece = &axel->pieces[i];
                            piece->collisionActive = true;
                            piece->timeCollisionActivated = Time;
                        }

                        foundMatch = true;
                    }
                }

                for (int32 i = 0; i < axel->pieceCount; i++) {
                    Piece *piece = &axel->pieces[i];
                    real32 elapsed = Time - piece->timeCollisionActivated;

                    if (elapsed >= CollisionDuration) {
                        piece->collisionActive = false;
                    }
                }

                real32 closestY = 999999;
                real32 dirY = 0;

                // find which piece is closest to the ball along the y axis and move to get it closer
                for (int32 i = 0; i < axel->pieceCount; i++) {
                    Piece *piece = &axel->pieces[i];

                    real32 yDiff = ball->position.y - piece->position.y;

                    if (Abs(yDiff) < closestY) {
                        closestY = Abs(yDiff);

                        if (yDiff < 0.5f) {
                            dirY = -1;
                        }
                        else if (yDiff > 0.5f) {
                            dirY = 1;
                        }
                    }
                }

                real32 speed = 8;
                axel->velocity = V2(0, dirY * speed);

                vec2 axelMin = V2(INFINITY);
                vec2 axelMax = V2(-INFINITY);
                
                for (int32 i = 0; i < axel->pieceCount; i++) {
                    Piece *piece = &axel->pieces[i];
                    piece->position = piece->position + (axel->velocity * DeltaTime);

                    vec2 pieceMax = piece->position + V2(PieceSize);

                    axelMin = Min(piece->position, axelMin);
                    axelMax = Max(pieceMax, axelMin);
                }

                // resolve collision with the top of the field
                real32 borderOverlap = 0;
                if (axelMin.y < 0) {
                    borderOverlap = axelMin.y;
                }
                else if (axelMax.y >= ArenaHeight) {
                    borderOverlap = axelMax.y - ArenaHeight;
                }

                for (int32 i = 0; i < axel->pieceCount; i++) {
                    Piece *piece = &axel->pieces[i];
                    piece->position.y -= borderOverlap;
                }
            }

            if (foundMatch) {
                // reset input for everything
                for (int i = 0; i < InputLength; i++) {
                    player->input[i] = 0;
                }
            
                player->inputIndex = 0;

                // @TODO: regenerate the strings for axels
            }
        }
    }

    {
        Ball *ball = &Foose.ball;

        ball->position = ball->position + ball->velocity * DeltaTime;

        // check for collisions
        vec2 min = ball->position;
        vec2 max = ball->position + V2(BallSize);

        if (min.y < 0) {
            ball->velocity.y *= -1;
        }
        else if (max.y >= ArenaHeight) {
            ball->velocity.y *= -1;
        }
        else if (min.x < 0) {
            ball->velocity.x *= -1;
        }
        else if (max.x >= ArenaWidth) {
            ball->velocity.x *= -1;
        }

        // check collision against player
        for (int t = 0; t < 2; t++) {
            Team *team = &Foose.teams[t];

            // find which direction we should move to have one of our pieces intercept the ball
            for (int a = 0; a < AxelCount; a++) {
                Axel *axel = &team->axels[a];

                for (int32 i = 0; i < axel->pieceCount; i++) {
                    Piece *piece = &axel->pieces[i];

                    vec2 pieceMin = piece->position;
                    vec2 pieceMax = piece->position + V2(PieceSize);

                    if (!piece->collisionActive) { continue; }

                    vec2 dir;
                    if (TestAABBAABB(min, max, pieceMin, pieceMax, &dir)) {
                        ball->position = ball->position + dir;

                        real32 force = 1.2f;
                        if (dir.x < 0) {
                            ball->velocity.x *= -1;
                            //ball->velocity.x *= force;
                        }
                        else if (dir.x > 0) {
                            ball->velocity.x *= -1;
                            //ball->velocity.x *= force;
                        }
                        else if (dir.y < 0) {
                            ball->velocity.y *= -1;
                            //ball->velocity.y *= force;
                        }
                        else if (dir.x > 0) {
                            ball->velocity.y *= -1;
                            //ball->velocity.y *= force;
                        }
                    }
                }
            }
        }
    }

    {
        Player *player = &Foose.player;
        DrawTextTile(V2(ArenaWidth / 2, ArenaHeight + 1), 5.0f, V4(1), false, "%.*s", player->inputIndex, player->input);
    }

    RenderTeam(&Foose.teams[0]);
    DrawGoal(&Foose.goals[0]);

    //RenderTeam(&Foose.teams[1]);

    DrawBall(&Foose.ball);
}
