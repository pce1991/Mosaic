
/*
  Need a way to find the address. clients get the server address then send it their address. 
  Server adds them to a list of clients

  I want to demonstrate a couple things with networking:
  one of the players is host (turn based prob)
  How you'd build your game if there is a server that feeds data to clients
 */

int32 InitAddress(sockaddr_in *addr, u8 a, u8 b, u8 c, u8 d, int16 port) {
    uint32 address = (a << 24) | (b << 16) | (c << 8) | d;

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(address);
    addr->sin_port = htons(port);

    return address;
}

int32 InitSocket(Socket *socketPtr, u8 a, u8 b, u8 c, u8 d, int16 port) {
    socketPtr->handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    uint32 address = (a << 24) | (b << 16) | (c << 8) | d;

    sockaddr_in *addr = &socketPtr->socketAddress;

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(address);
    addr->sin_port = htons(port);

    int32 bindSuccess = bind(socketPtr->handle, (const sockaddr*)addr, sizeof(sockaddr_in));
    if (bindSuccess != 0) {
        Print("failed to bind socket!");
    }

    DWORD nonBlocking = 1;
    int32 nonBlockingSuccess = ioctlsocket(socketPtr->handle, FIONBIO, &nonBlocking);

    if (nonBlockingSuccess != 0) {
        Print("Failed to set non-blocking!");
    }

    return address;
}

int32 SendPacket(Socket *socket, void *packetData, uint32 packetSize) {
    return sendto(socket->handle, (char *)packetData, packetSize, 0, (sockaddr *)&socket->socketAddress, sizeof(sockaddr_in));
}

int32 ReceivePacket(Socket *socket, void *buffer, uint32 bufferSize, Socket *fromSocket) {
    int32 fromSize = sizeof(sockaddr_in);
    
    return recvfrom(socket->handle, (char *)buffer, bufferSize, 0, (sockaddr *)&fromSocket->socketAddress, &fromSize);
}


void AllocateNetworkInfo(NetworkInfo *info, int32 receivingCount, int32 sendingCount) {
    info->receivingSocketCount = receivingCount;
    info->receivingSockets = (Socket *)malloc(receivingCount * sizeof(Socket));

    info->sendingSocketCount = sendingCount;
    info->sendingSockets = (Socket *)malloc(sendingCount * sizeof(Socket));
}


// Probably two types of packets: network and client
// I mean really we can have any number of types of packets, as long as we always receive the max size
// We'll have different updates based on if the game is a server or a client.
// This way the server can send out the chunky information and clients know what to receieve.


void ReceivePackets() {

    // @TODO: need to do this loop for each of the receiving sockets
    // which means we also need to a way to specify which packet came from where

    NetworkInfo *info = &Game->networkInfo;
    
    DynamicArrayClear(&info->packetsReceived);

    while (true) {
        GamePacket *packet = PushBackPtr(&Game->networkInfo.packetsReceived);
        
        Socket sender;
        Socket fromSocket;
        int32 bytesReceived = ReceivePacket(&Game->networkInfo.receivingSockets[0], (u8 *)packet, sizeof(GamePacket), &fromSocket);

        if (bytesReceived <= 0) {
            // @WINDOWS
            int32 error = WSAGetLastError();
            // 10035 is a non-fatal error you get on non-blocking when there isnt anything found.
            if (error != 10035) {
                Print("recvfrom error: %d", error);
            }
            
            break;
        }
            
        int32 fromAddress = ntohl(fromSocket.socketAddress.sin_addr.s_addr);
        int32 fromPort = ntohs(fromSocket.socketAddress.sin_port);
    }
}

void SendPackets() {
    NetworkInfo *info = &Game->networkInfo;

    GamePacket packet = {};
    packet.type = GamePacketType_String;
    char *str = "Hello There.";
    memcpy(packet.data, str, strlen(str));

    PushBack(&info->packetsToSend, packet);

    for (int i = 0; i < info->packetsToSend.count; i++) {
        int32 packetSize = sizeof(GamePacket);
        int32 bytesSent = SendPacket(&Game->networkInfo.sendingSockets[0], &info->packetsToSend[i], packetSize);
    }

    DynamicArrayClear(&info->packetsToSend);
}