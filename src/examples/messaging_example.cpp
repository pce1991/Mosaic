
// TODO: do a think like scenes. We'll just manually add them. it'll be a lot easier to just switch between
// what scene is running and get the data. 

struct MessagingExample {
    char message[128];
};

// @PERF: does allocating the struct globally affect the cache?
MessagingExample myData = {};

void MyInit() {
    
};

void MyGameUpdate() {

    NetworkInfo *networking = &Game->networkInfo;

    myData.message[0] = ((Game->frame) % 64) + 64;
    myData.message[1] = 0;

    if (InputPressed(Input, Input_Return)) {
        GamePacket packet = {};
        packet.type = GamePacketType_String;
        memcpy(packet.data, myData.message, strlen(myData.message));

        PushBack(&networking->packetsToSend, packet);
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
