
// TODO: do a think like scenes. We'll just manually add them. it'll be a lot easier to just switch between
// what scene is running and get the data. 

struct MessagingExample {
    int32 messageCount;
    char message[128];

    real32 timeReceived;
    char messageReceived[128];
};

// @PERF: does allocating the struct globally affect the cache?
MessagingExample myData = {};

void MyInit() {
    
};

// @TODO: show the last time stamp of message
//        send messages notifying activity, and when they've signed off (n seconds since ping)

void MyGameUpdate() {

    NetworkInfo *networking = &Game->networkInfo;

    // myData.message[0] = ((Game->frame) % 64) + 64;
    // myData.message[1] = 0;

    if (InputPressed(Input, Input_Return)) {
        GamePacket packet = {};
        packet.type = GamePacketType_String;
        memcpy(packet.data, myData.message, strlen(myData.message));

        PushBack(&networking->packetsToSend, packet);

        myData.messageCount = 0;
        memset(myData.message, 0, 128);
    }
    else if (InputHeld(Input, Input_Backspace)) {
        if (myData.messageCount > 0) {
            myData.message[--myData.messageCount] = 0;
        }
    }
    else if (Input->charCount > 0) {
        myData.message[myData.messageCount++] = Input->inputChars[0];
    }

    if (networking->packetsReceived.count > 0) {
        GamePacket packet = networking->packetsReceived[0];

        Print("packet");

        if (packet.type == GamePacketType_String) {
            memcpy(myData.messageReceived, packet.data, strlen((char *)packet.data));

            myData.timeReceived = Game->time;
        }
    }

    DrawText(V2(-4.0f, 0.0f), 8.0, V4(1), "my message: %s", myData.message);

    if (strlen(myData.messageReceived) > 0) {
        DrawText(V2(-4.0f, -1.5f), 8.0, V4(1), "received: %s", myData.messageReceived);
    }
    else {
        DrawText(V2(-4.0f, -1.5f), 8.0, V4(1), "NO MESSAGE RECEIVED");
        //DrawText(V2(0.0f, 0.0), 8.0, V4(1), "me: ");
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
