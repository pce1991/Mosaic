
// TODO: do a think like scenes. We'll just manually add them. it'll be a lot easier to just switch between
// what scene is running and get the data.

// use ipconfig to find this for whatever machine you want to host your server on.
const uint32 ServerAddress = MakeAddressIPv4(192, 168, 1, 35);
const uint16 Port = 30000;

struct UserInfo {
    uint32 address;
    real32 lastPingTime;
};

struct Message {
    real32 timeSent;

    // @NOTE: this is so we have arbitrary length messages.
    char *message;
    int32 messageLength;
};

struct ServerData {
    DynamicArray<UserInfo> users;

    // Maybe you would want to store these on the user if you want to easily search messages
    // from certain users. But then you'd want a way to order things efficiently.
    DynamicArray<Message> messages;
};


struct MessagingExample {
    bool isServer;
    
    int32 messageCount;
    char message[64];

    real32 timeReceived;
    char messageReceived[64];

    real32 lastBackspaceTime;

    bool gotPing;
    real32 lastTimeGotPing;

    ServerData server;
};

// @PERF: does allocating the struct globally affect the cache?
MessagingExample myData = {};

void MyInit() {
    Socket sendingSocket = {};
    InitSocket(&sendingSocket, ServerAddress, Port);
    PushBack(&Game->networkInfo.sendingSockets, sendingSocket);

    myData.isServer = GetMyAddress() == ServerAddress;
}

// @TODO: show the last time stamp of message
//        send messages notifying activity, and when they've signed off (n seconds since ping)
//        multiple users connected to a server.

void ServerUpdate() {
    NetworkInfo *networking = &Game->networkInfo;
    ServerData *server = &myData.server;

    for (int i = 0; i < server->users.count; i++) {
        Print("user %d last ping %f", server->users[i].address, server->users[i].lastPingTime);
    }
    
    for (int i = 0; i < networking->packetsReceived.count; i++) {
        ReceivedPacket received = networking->packetsReceived[i];

        if (received.packet.type == GamePacketType_Ping) {
            bool found = false;
            
            for (int j = 0; j < server->users.count; j++) {
                UserInfo *user = &server->users[j];
                if (received.fromAddress == user->address) {
                    found = true;

                    user->lastPingTime = Game->time;
                    
                    break;
                }
            }

            if (!found) {
                UserInfo user = {};
                user.address = received.fromAddress;
                user.lastPingTime = Game->time;
                
                PushBack(&server->users, user);

                // @MAYBE: need 
                Socket socket = {};
                InitSocket(&socket, received.fromAddress, Port);

                PushBack(&Game->networkInfo.sendingSockets, socket);
            }
        }
        else {
            GamePacket toSend = received.packet;

            // @TODO: we actually only want to send this data to certain clients,
            // it shouldnt be necessary to send it to the sender, that should just
            // be stored on their machine.
            // Right now we just send a message out to everybody but that isnt actually what we want.
            memcpy(toSend.data, received.packet.data, 256);

            PushBack(&Game->networkInfo.packetsToSend, toSend);
        }
    }
}

void MyGameUpdate() {

    NetworkInfo *networking = &Game->networkInfo;

    if (myData.isServer) {
        ServerUpdate();
    }

    // @TODO: now we have users connected to a server, so we need to display messages sent to the server
    // back to the users.
    // Client A sends message to server, now client B needs to see it.
    // I think its fine for the clients to keep the data on whats been sent right?

    GamePacket packet = {};
    packet.type = GamePacketType_Ping;
    PushBack(&networking->packetsToSend, packet);
    
    if (InputPressed(Input, Input_Return)) {
        GamePacket packet = {};
        packet.type = GamePacketType_String;
        memcpy(packet.data, myData.message, strlen(myData.message));

        PushBack(&networking->packetsToSend, packet);

        myData.messageCount = 0;
        memset(myData.message, 0, 128);
    }
    else if (InputPressed(Input, Input_Backspace)) {
        if (myData.messageCount > 0) {
            myData.message[--myData.messageCount] = 0;
            myData.lastBackspaceTime = Game->time;
        }
    }
    else if (InputHeld(Input, Input_Backspace)) {
        real32 timeSince = Game->time - myData.lastBackspaceTime;

        if (timeSince > 0.08f) {
            if (myData.messageCount > 0) {
                myData.message[--myData.messageCount] = 0;
            }

            myData.lastBackspaceTime = Game->time;
        }
    }
    else if (Input->charCount > 0) {
        myData.message[myData.messageCount++] = Input->inputChars[0];
    }

#if 1
    if (networking->packetsReceived.count > 0) {
        for (int i = 0; i < networking->packetsReceived.count; i++) {
            GamePacket packet = networking->packetsReceived[i].packet;

            if (packet.type == GamePacketType_String) {
                memcpy(myData.messageReceived, packet.data, strlen((char *)packet.data));

                myData.timeReceived = Game->time;
            }
            if (packet.type == GamePacketType_Ping) {
                // @TODO: if we get a ping for the first time we want to figure out who the user is
                // and display their name.
                myData.lastTimeGotPing = Game->time;
            }
        }
    }
#endif

    DrawTextScreen(V2(800, 100), 32, V4(1), true, "MESSENGER APPETIZER");

    DrawText(V2(-4.0f, 1.0f), 0.2, V4(1), "to send: %s", myData.message);

    // @TODO: It'd be nice to get the position of the last glyph so we can draw a quad there,
    // and to know how big each glyph is so we can advance properly.
    DrawRectScreen(V2(400, 400), V2(24, 32), V4(0.0f, 0.6f + (0.2f * sinf(Game->time * 8)), 0.4f, 0.8f));    

    DrawText(V2(-4.0f, -0.5f), 0.2, V4(1), "received: %s", myData.messageReceived);
    
    real32 timeSincePing = Game->time - myData.lastTimeGotPing;
    if (!myData.gotPing || timeSincePing > 5.0) {
        DrawText(V2(-4.0f, -1.5f), 0.2, V4(1, 0, 0, 1), "No user connected");
    }

    char name[256] = {};
    gethostname(name, 256);

    //Print("name %s", name);

    sockaddr addr;
    int32 size = sizeof(sockaddr);
    int32 result = getpeername(networking->sendingSockets[0].handle, &addr, &size);

    // if (result >= 0) {
    //     Print("asdf");    
    // }
    // else {
    //     int32 error = WSAGetLastError();
    //     Print("recvfrom error: %d", error);
    // }
}
