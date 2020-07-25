
// use ipconfig to find this for whatever machine you want to host your server on.
const uint32 ServerAddress = MakeAddressIPv4(192, 168, 1, 35);
const uint16 Port = 30000;

const uint32 PacketID = Hash("Versus Platformer");

const real64 TICK_HZ = 1.0 / 60.0;

struct Player {
    int32 id;
    
    vec2 position;
    vec2 velocity;
    Rect rect;
    vec4 color;
};

struct Slab {
    int32 id;
    
    vec2 position;
    Rect rect;
};

struct UserInfo {
    uint32 address;
    real32 lastPingTime;

    bool ready;
    bool levelSpawned;
};


struct Server {
    DynamicArray<UserInfo> users;

    // @TODO: buffer of inputs per user

    real32 timeAccumulator;
};

struct Platformer {
    bool ready;
    bool levelCreated;
    
    DynamicArray<Player> players;
    int32 playerID; // instead of an index in case we want drop in/out

    DynamicArray<Slab> slabs;

    bool isServer;
    Server server;
};

Platformer *myData = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(Platformer));

    myData = (Platformer *)Game->myData;
    memset(myData, 0, sizeof(Platformer));

    myData->isServer = GetMyAddress() == ServerAddress;
    
    Socket sendingSocket = {};
    InitSocket(&sendingSocket, ServerAddress, Port);
    PushBack(&Game->networkInfo.sendingSockets, sendingSocket);
}


void SpawnLevel() {
    {
        Slab slab = {};

        slab.position = V2(0);
        slab.rect.min = V2(-1, -1);
        slab.rect.min = V2(1, 1);

        PushBack(&myData->slabs, slab);
    }
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
        for (int j = 0; j < server->users.count; j++) {
            UserInfo *u = &server->users[j];
            if (received->fromAddress == u->address) {
                user = u;
                break;
            }
        }

        if (received->packet.type == GamePacketType_Ping) {
            if (user != NULL) {
                user->lastPingTime = Game->time;
            }
            else {
                UserInfo user = {};
                user.address = received->fromAddress;
                user.lastPingTime = Game->time;

                if (received->packet.data[0]) {
                    user.ready = true;
                }
                
                PushBack(&server->users, user);
            }
        }

        if (received->packet.type == GamePacketType_String) {
            if (strcmp((char *)received->packet.data, "SpawnedLevel")) {
                user->levelSpawned = true;
            }
        }

        if (received->packet.type == GamePacketType_Input) {
            
        }
    }

    bool allReady = true;
    for (int i = 0; i < server->users.count; i++) {
        UserInfo *u = &server->users[i];

        allReady &= u->ready;
    }

    if (allReady) {
        // Spawn level on client machines.

        GamePacket packet = {};
        packet.type = GamePacketType_String;

        const char *str = "SpawnLevel";
        memcpy(packet.data, str, strlen(str));

        PushBack(&network->packetsToSend, packet);

        SpawnLevel();
        // @TODO: need to tell the clients which player they are.
        //        that suggests we want something more than just a string message for this.
    }
}


void ClientUpdate() {

    NetworkInfo *network = &Game->networkInfo;

    for (int i = 0; i < network->packetsReceived.count; i++) {
        ReceivedPacket *received = &network->packetsReceived[i];

        if (received->packet.type == GamePacketType_String) {
            if (strcmp((char *)received->packet.data, "SpawnLevel")) {
                SpawnLevel();

                GamePacket packet = {};
                packet.type = GamePacketType_String;

                const char *str = "SpawnedLevel";
                memcpy(packet.data, str, strlen(str));

                PushBack(&network->packetsToSend, packet);
            }
        }
    }

    if (InputPressed(Input, Input_Space)) {
        myData->ready = !myData->ready;

        {
            GamePacket packet = {};
            packet.type = GamePacketType_Ping;
            packet.data[0] = myData->ready;

            PushBack(&network->packetsToSend, packet);
        }
    }
    
    if (!myData->ready) {
        DrawTextScreen(&Game->serifFont, V2(800, 100), 32, V4(1), true, "PRESS SPACE TO READY");
    }
    else {
        DrawTextScreen(&Game->serifFont, V2(800, 100), 32, V4(1), true, "PRESS SPACE TO UNREADY");
        
        DrawTextScreen(&Game->serifFont, V2(800, 200), 32, V4(1), true, "WAITING ON OTHER PLAYERS");
    }
    
    for (int i = 0; i < myData->slabs.count; i++) {
        Slab *plat = &myData->slabs[i];

        vec2 scale = (plat->rect.max - plat->rect.min) * 0.5f;
        DrawRect(plat->position, scale, V4(0.3f, 0.3f, 0.3f, 1.0f));
    }
    
    for (int i = 0; i < myData->players.count; i++) {
        Player *player = &myData->players[i];

        vec2 scale = (player->rect.max - player->rect.min) * 0.5f;
        DrawRect(player->position, scale, player->color);
    }
}

void MyGameUpdate() {
    if (myData->isServer) {
        myData->server.timeAccumulator += Game->deltaTime;

        while (myData->server.timeAccumulator >= TICK_HZ) {
            ServerUpdate();
            
            myData->server.timeAccumulator -= TICK_HZ;
            Print("tick server at %f accum: %f dt: %f", Game->time, myData->server.timeAccumulator, Game->deltaTime);
        }
    }
    else {
        ClientUpdate();
    }
}

