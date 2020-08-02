
// use ipconfig to find this for whatever machine you want to host your server on.
const uint32 ServerAddress = MakeAddressIPv4(192, 168, 1, 35);
const uint16 Port = 30000;

struct UserInfo {
    uint32 address;
    real32 lastPingTime;

    char *name;
};

struct Message {
    real32 timeSent;

    char *userName;
    int32 userNameLength;

    // @NOTE: this is so we have arbitrary length messages.
    char *string;
    int32 length;
};

struct ServerData {
    DynamicArray<UserInfo> users;

    // Maybe you would want to store these on the user if you want to easily search messages
    // from certain users. But then you'd want a way to order things efficiently.
    DynamicArray<Message> messages;
};


struct MessagingExample {
    bool isServer;

    DynamicArray<Message> messages;
    
    int32 messageCount;
    char message[64];

    real32 timeReceived;
    char messageReceived[64];

    real32 lastBackspaceTime;

    bool gotPing;
    real32 lastTimeGotPing;

    ServerData server;

    bool enteredName;
    char name[64];
};

// @PERF: does allocating the struct globally affect the cache?
MessagingExample myData = {};

void MyInit() {
    Socket sendingSocket = {};
    InitSocket(&sendingSocket, ServerAddress, Port);
    PushBack(&Game->networkInfo.sendingSockets, sendingSocket);

    myData.isServer = GetMyAddress() == ServerAddress;
}

void ServerUpdate() {
    NetworkInfo *networking = &Game->networkInfo;
    ServerData *server = &myData.server;

    Print("packets received %u", networking->packetsReceived.count);

    Print("user count %u", server->users.count);
    for (int i = 0; i < server->users.count; i++) {
        Print("user %s %u last ping %f", server->users[i].name, server->users[i].address, server->users[i].lastPingTime);
    }

    {
        GamePacket toSend = {};
        toSend.type = GamePacketType_Ping;
        PushBack(&networking->packetsToSend, toSend);
    }
    
    for (int i = 0; i < networking->packetsReceived.count; i++) {
        ReceivedPacket received = networking->packetsReceived[i];

        UserInfo *user = NULL;
        for (int j = 0; j < server->users.count; j++) {
            UserInfo *u = &server->users[j];
            if (received.fromAddress == u->address) {
                user = u;
                break;
            }
        }


        if (received.packet.type == GamePacketType_Ping) {
            if (user != NULL) {
                user->lastPingTime = Game->time;
            }
            else {
                UserInfo user = {};
                user.address = received.fromAddress;
                user.lastPingTime = Game->time;
                
                PushBack(&server->users, user);

                // @HACK: really shouldnt need to check to make sure we didnt get a message from ourselves.
                if (received.fromAddress != networking->receivingSocket.address) {
                    // @MAYBE: need 
                    Socket socket = {};
                    InitSocket(&socket, received.fromAddress, Port);

                    PushBack(&Game->networkInfo.sendingSockets, socket);
                }
            }
        }

        if (received.packet.type == GamePacketType_Username) {
            if (user) {
                int32 nameLen = strlen((char *)received.packet.data);
                user->name = (char *)malloc(nameLen + 1);
                memcpy(user->name, received.packet.data, nameLen);
                user->name[strlen((char *)received.packet.data)] = 0;
                    
                break;
            }
        }

        // Its a little weird that we get a string and then send out a message,
        // but the idea is that there's no need for the client to send their name since we
        // have it from the IP address.
        if (received.packet.type == GamePacketType_String) {
            GamePacket toSend = received.packet;
            toSend.type = GamePacketType_Message;

            // @TODO: we actually only want to send this data to certain clients,
            // it shouldnt be necessary to send it to the sender, that should just
            // be stored on their machine.
            // Right now we just send a message out to everybody but that isnt actually what we want.

            int32 nameLen = strlen(user->name);
            memcpy(toSend.data, &nameLen, sizeof(int32));

            memcpy(toSend.data + sizeof(int32), user->name, nameLen);

            int32 sizeLeft = 256 - nameLen - sizeof(int32);
            memcpy(toSend.data + sizeof(int32) + nameLen, received.packet.data, sizeLeft);

            PushBack(&Game->networkInfo.packetsToSend, toSend);
        }
    }

    Print("packets sent from server %u", networking->packetsToSend.count);
}

void MyGameUpdate() {
    NetworkInfo *networking = &Game->networkInfo;

    if (myData.isServer) {
        ServerUpdate();
    }

    GamePacket packet = {};
    packet.type = GamePacketType_Ping;
    PushBack(&networking->packetsToSend, packet);

    if (InputPressed(Input, Input_Return)) {

        if (myData.enteredName) {
            GamePacket packet = {};
            packet.type = GamePacketType_String;
            memcpy(packet.data, myData.message, strlen(myData.message));

            PushBack(&networking->packetsToSend, packet);

            myData.messageCount = 0;
            memset(myData.message, 0, 64);
        }
        else {
            GamePacket packet = {};
            packet.type = GamePacketType_Username;
            memcpy(packet.data, myData.message, strlen(myData.message));

            PushBack(&networking->packetsToSend, packet);

            myData.messageCount = 0;
            memset(myData.message, 0, 64);

            myData.enteredName = true;
        }
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
        // < 63 because the last character must be a zero
        if (myData.messageCount + 1 < 63) {
            myData.message[myData.messageCount++] = Input->inputChars[0];
        }
    }

#if 1
    if (networking->packetsReceived.count > 0) {
        for (int i = 0; i < networking->packetsReceived.count; i++) {
            GamePacket packet = networking->packetsReceived[i].packet;

            if (packet.type == GamePacketType_Message) {
                Message m = {};
                m.timeSent = Game->time;

                int32 nameLen = *(int32 *)packet.data;

                m.userName = (char *)malloc(nameLen);
                memcpy(m.userName, packet.data + sizeof(int32), nameLen);

                m.userNameLength = nameLen;

                m.length = strlen((char *)(packet.data + sizeof(int32) + nameLen));
                m.string = (char *)malloc(m.length);

                memcpy(m.string, packet.data + sizeof(int32) + nameLen, m.length);

                PushBack(&myData.messages, m);
            }
            if (packet.type == GamePacketType_Ping) {
                // @TODO: if we get a ping for the first time we want to figure out who the user is
                // and display their name.
                myData.lastTimeGotPing = Game->time;
                myData.gotPing = true;
            }
        }
    }
#endif

    DrawTextScreen(&Game->serifFont, V2(800, 100), 32, V4(1), true, "MESSENGER APPETIZER");

    vec2 entryCursor = V2(300, 800);

    vec2 *positionsBuffer = NULL;
    int32 charCount = 0;
    if (!myData.enteredName) {
        charCount = DrawTextScreen(&Game->monoFont, entryCursor, 16.0f, V4(1), false, &positionsBuffer, "Enter your name and press enter: %s", myData.message);
    }
    else {
        charCount = DrawTextScreen(&Game->monoFont, entryCursor, 16.0f, V4(1), false, &positionsBuffer, "to send: %s", myData.message);
    }

    // @HACK: this is not exactly the position we'd want to draw the cursor.
    // I'm fudging the numbers a bit so it looks right but it isnt really.
    vec2 lastPosition = positionsBuffer[charCount - 1];
    lastPosition.y = 0;
    DrawRectScreen(lastPosition + entryCursor + V2(12, 4), V2(24, 32), V4(0.0f, 0.6f + (0.2f * sinf(Game->time * 8)), 0.4f, 0.8f));

    vec2 cursor = V2(300, 720);
    for (int i = myData.messages.count - 1; i >= 0; i--) {
        if (cursor.y < 0) {
            break;
        }

        // @TODO: if we want to layout text with different colors we have a couple options:
        // a call to DrawText for the username (one color) and then a call for the text.
        // That means we need to know the dimensions after one DrawText call.
        // The other option is to pass in an array of colors (which seems fine but eventually we want dimensions)
        Message m = myData.messages[i];
        DrawTextScreen(&Game->monoFont, cursor, 16.0f, V4(1), false, "%.2f %.*s: %.*s", m.timeSent, m.userNameLength, m.userName, m.length, m.string);

        cursor.y -= 40;
    }
    
    real32 timeSincePing = Game->time - myData.lastTimeGotPing;
    if (!myData.gotPing || timeSincePing > 5.0) {
        DrawTextScreen(&Game->monoFont, V2(1000, 200), 32.0f, V4(1, 0, 0, 1), "No user connected");
    }
}
