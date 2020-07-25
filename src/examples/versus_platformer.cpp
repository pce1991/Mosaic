
// @TODO: what happens if a client connects after a game is finished? we should be able to restart game


// use ipconfig to find this for whatever machine you want to host your server on.
const uint32 ServerAddress = MakeAddressIPv4(192, 168, 1, 35);
const uint16 Port = 30000;
// @TODO: is it possible I can run the server on the same machine that a user plays on by changing the port number?
// like the server is port 30000 and the client app uses port 30001
// If so then we'd need to compare the IP address and the port number

const uint32 PacketID = Hash("Pong");

const real64 TICK_HZ = 1.0 / 60.0;

struct Player {
    int32 id;
    
    vec2 position;
    vec2 velocity;
    Rect rect;
    vec4 color;

    real32 timeLastHit;
    uint8 score;
};

struct Ball {
    vec2 position;
    vec2 velocity;

    Rect rect;
    vec4 color;
};

struct UserInfo {
    uint32 address;
    real32 lastPingTime;

    bool ready;
    bool levelSpawned;
};

// @NOTE: we use these structs for conveinent casting of packet.data
struct InputPacket {
    // @GACK: sorta odd that the client doesnt send their id and we figure it out based on IP address on server
    int32 id;
    InputID input;
};

struct ClientPacket {
    vec2 positions[2];
    uint8 scores[2];
    bool collided[2];

    vec2 ballPosition;
    vec2 ballVelocity;
};

// @TODO: allow multiple servers per instance of game so you could have N players playing M games of pong
struct Server {
    DynamicArray<UserInfo> users;

    DynamicArray<InputPacket> inputs;

    real32 timeAccumulator;
};

struct Pong {
    bool ready;
    bool levelCreated;

    bool playing;

    Ball ball;
    Player players[2];
    int32 playerID; // this tells the client which player it is

    bool isServer;
    Server server;
};

Pong *myData = NULL;

Rect PaddleRect = {};
Rect BallRect = {};

void MyInit() {
    Game->myData = malloc(sizeof(Pong));

    myData = (Pong *)Game->myData;
    memset(myData, 0, sizeof(Pong));

    myData->isServer = GetMyAddress() == ServerAddress;

    if (!myData->isServer) {
        Socket sendingSocket = {};
        InitSocket(&sendingSocket, ServerAddress, Port);
        PushBack(&Game->networkInfo.sendingSockets, sendingSocket);
    }

    PaddleRect.min = V2(-0.2f, -0.8f);
    PaddleRect.max = V2(0.2f, 0.8f);

    BallRect.min = V2(-0.1f, -0.1f);
    BallRect.max = V2(0.1f, 0.1f);
}


// @TODO: get this running at a fixed timestep
void ServerUpdate() {
    NetworkInfo *network = &Game->networkInfo;

    Server *server = &myData->server;
    
    for (int i = 0; i < network->packetsReceived.count; i++) {
        ReceivedPacket *received = &network->packetsReceived[i];

        if (received->packet.id != PacketID) {
            continue;
        }

        UserInfo *user = NULL;
        int32 userIndex = 0;
        for (int j = 0; j < server->users.count; j++) {
            UserInfo *u = &server->users[j];
            if (received->fromAddress == u->address) {
                user = u;
                userIndex = j;
                break;
            }
        }

        if (received->packet.type == GamePacketType_Ping) {
            if (user != NULL) {
                user->lastPingTime = Game->time;

                //Print("user %d last ping %f", userIndex, user->lastPingTime);

                if (received->packet.data[0]) {
                    user->ready = true;
                }
            }
            else {
                UserInfo u = {};
                u.address = received->fromAddress;
                u.lastPingTime = Game->time;

                if (received->packet.data[0]) {
                    u.ready = true;
                }
                
                PushBack(&server->users, u);

                Socket socket = {};
                InitSocket(&socket, received->fromAddress, Port);
                PushBack(&Game->networkInfo.sendingSockets, socket);

                user = &server->users[server->users.count - 1];
                userIndex = server->users.count - 1;
            }
        }

        if (received->packet.type == GamePacketType_String) {
            if (strcmp((char *)received->packet.data, "SpawnedLevel") == 0) {
                user->levelSpawned = true;
            }
        }

        if (received->packet.type == GamePacketType_Input) {
            InputPacket packet = *(InputPacket *)received->packet.data;
            packet.id = userIndex;
            PushBack(&server->inputs, packet);
        }
    }

    int32 readyCount = 0;
    for (int i = server->users.count - 1; i >= 0; i--) {
        UserInfo *u = &server->users[i];

        if (Game->time - u->lastPingTime > 5.0f) {
            myData->playing = false;
            RemoveAtIndex(&server->users, i);
        }
        else if (u->ready) {
            readyCount++;
        }
    }

    real32 ballMinSpeed = 2.0f;
    real32 ballMaxSpeed = 5.0f;

    real32 paddleMaxSpeed = 3.6f;
    real32 paddleAccel = 30.0f;
    real32 paddleDecel = 40.0f;
    
    if (readyCount == 1 && !myData->playing) {
        myData->playing = true;

        for (int i = 0; i < 2; i++) {
            Player * player = &myData->players[i];

            player->velocity = V2(0);
            player->score = 0;

            if (i == 0) {
                player->position = V2(-6.5f, 0.0f);
            }
            else {
                player->position = V2(6.5f, 0.0f);
            }

            player->rect = PaddleRect;
        }

        Ball *ball = &myData->ball;
        
        ball->position = V2(0);

        ball->rect = BallRect;

        bool even = RandiRange(0, 10) % 2 == 0;
        real32 x = 1;
        if (even) {
            x = -1;
        }
            
        ball->velocity = V2(x, 0) * ballMinSpeed;
    }

    if (myData->playing) {

        GamePacket packet = {};
        packet.id = PacketID;
        packet.type = GamePacketType_Pong;

        ClientPacket *clientData = (ClientPacket *)packet.data;
        
        for (int i = 0; i < server->inputs.count; i++) {
            InputPacket input = server->inputs[i];

            Player *player = NULL;
            for (int j = 0; j < 2; j++) {
                if (input.id == myData->players[j].id) {
                    player = &myData->players[j];
                }
            }

            if (input.input == Input_Up) {
                player->velocity.y += paddleAccel * TICK_HZ;
            }
            else if (input.input == Input_Down) {
                player->velocity.y += -paddleAccel * TICK_HZ;
            }
            else if (input.input == Input_None) {
                if (player->velocity.y != 0.0f) {
                    real32 startVel = player->velocity.y;

                    if (startVel > 0) {
                        player->velocity.y -= paddleDecel * TICK_HZ;

                        if (player->velocity.y < 0) {
                            player->velocity.y = 0;
                        }
                    }
                    else if (startVel < 0) {
                        if (player->velocity.y > 0) {
                            player->velocity.y = 0;
                        }
                        player->velocity.y = 0;
                    }
                }
            }

            player->velocity.y = Clamp(player->velocity.y, -paddleMaxSpeed, paddleMaxSpeed);
        }

        for (int i = 0; i < 2; i++) {
            Player * player = &myData->players[i];

            player->position = player->position + player->velocity * TICK_HZ;

            vec2 min = player->position + player->rect.min;
            vec2 max = player->position + player->rect.max;

            if (min.y < -4.5) {
                real32 diff = -4.5 - min.y;
                player->position.y += diff;
            }
            
            if (max.y > 4.5) {
                real32 diff = 4.5 - max.y;
                player->position.y += diff;
            }
        }

        Ball *ball = &myData->ball;
        ball->position = ball->position + ball->velocity * TICK_HZ;


        for (int i = 0; i < 2; i++) {
            Player *player = &myData->players[i];
            clientData->scores[i] = player->score;

            Print("sending score of %d for player %d", player->score, i);

            clientData->positions[i] = player->position;

            // @BUG: possible the ball gets stuck on the paddle in some cases?
            vec2 dir;
            if (RectTest(ball->rect, player->rect, ball->position, player->position, &dir)) {
                clientData->collided[i] = true;
                ball->position = ball->position + dir;
                
                ball->velocity.x *= -1;
                ball->velocity.y = player->velocity.y * 1.1f;
            }

            clientData->ballPosition = ball->position;
            clientData->ballVelocity = ball->velocity;
        }

        if (ball->position.y > 4.4) {
            ball->velocity.y *= -1;
            //Print("hit top wall %f", ball->velocity.y);
            ball->position.y = 4.4f;
        }
        if (ball->position.y < -4.4) {
            ball->velocity.y *= -1;
            //Print("hit bottom wall %f", ball->velocity.y);
            ball->position.y = -4.4f;
        }


        bool resetBall = false;
        if (ball->position.x < -8) {
            myData->players[0].score++;
            resetBall = true;
        }
        if (ball->position.x > 8) {
            myData->players[1].score++;
            resetBall = true;
        }

        if (resetBall) {
            ball->position = V2(0);

            bool even = RandiRange(0, 10) % 2 == 0;
            real32 x = 1;
            if (even) {
                x = -1;
            }
            
            ball->velocity = V2(x, 0) * ballMinSpeed;
        }

        clientData->ballPosition = ball->position;
        clientData->ballVelocity = ball->velocity;
        
        PushBack(&network->packetsToSend, packet);
    }
}


void ClientUpdate() {

    NetworkInfo *network = &Game->networkInfo;

    for (int i = 0; i < network->packetsReceived.count; i++) {
        ReceivedPacket *received = &network->packetsReceived[i];

        if (received->packet.id != PacketID) {
            continue;
        }

        ClientPacket *data = (ClientPacket *)received->packet.data;
        if (received->packet.type == GamePacketType_Pong) {
            for (int j = 0; j < 2; j++) {
                Player *player = &myData->players[j];
                player->position = data->positions[j];

                if (data->collided[i]) {
                    player->timeLastHit = Game->time;
                }

                player->score = data->scores[j];
            }

            myData->ball.position = data->ballPosition;
            myData->ball.velocity = data->ballVelocity;
        }
    }

    GamePacket packet = {};
    packet.id = PacketID;
    packet.type = GamePacketType_Ping;
            
    // @TODO: send this every frame in case the server
    // a) dropped it or b) wasnt started yet (unlikely in production)
    // @TODO: we also need to ping every frame so that the server knows if we we exist 
    if (InputPressed(Input, Input_Space)) {
        myData->ready = !myData->ready;

        packet.data[0] = myData->ready;
    }
    
    PushBack(&network->packetsToSend, packet);

    // @TODO: instead we should draw one paddle and let you move around to signal readiness
    if (!myData->ready) {
        DrawTextScreen(&Game->serifFont, V2(800, 100), 32, V4(1), true, "PRESS SPACE TO READY");
    }

    {
        GamePacket packet = {};
        packet.id = PacketID;
        packet.type = GamePacketType_Input;

        ((InputPacket *)packet.data)->input = Input_None;


        if (InputHeld(Input, Input_Up)) {
            ((InputPacket *)packet.data)->input = Input_Up;
        }
        if (InputHeld(Input, Input_Down)) {
            ((InputPacket *)packet.data)->input = Input_Down;
        }

        PushBack(&network->packetsToSend, packet);
    }

    for (int i = 0; i < 2; i++) {
        Player *player = &myData->players[i];

        player->rect = PaddleRect;
        
        vec2 scale = (player->rect.max - player->rect.min) * 0.5f;
        DrawRect(player->position, scale, V4(1));
    }

    // @TODO: draw a trail based on velocity.
    Ball *ball = &myData->ball;
    ball->rect = BallRect;
    vec2 scale = (ball->rect.max - ball->rect.min) * 0.5f;

    real32 speedT = Length(ball->velocity) / 4.0f;
    vec4 ballColor = Lerp(V4(0.3f, 0.8f, 0.0f, 1.0f), V4(0.8f, 0.4f, 0.0f, 1.0f), speedT);
    
    DrawRect(ball->position, scale, ballColor);

    for (int i = 0; i < 2; i++) {
        Player *player = &myData->players[i];
        if (i == 0) {
            DrawText(&Game->monoFont, V2(5, 3.75f), 0.6f, V4(1), "%d", player->score);
        }
        else {
            DrawText(&Game->monoFont, V2(-5, 3.75f), 0.6f, V4(1), "%d", player->score);
        }
    }
}

void MyGameUpdate() {
    if (myData->isServer) {
        myData->server.timeAccumulator += Game->deltaTime;

        while (myData->server.timeAccumulator >= TICK_HZ) {
            ServerUpdate();
            
            myData->server.timeAccumulator -= TICK_HZ;
            //Print("tick server at %f accum: %f dt: %f", Game->time, myData->server.timeAccumulator, Game->deltaTime);
        }
    }
    else {
        ClientUpdate();
    }
}

