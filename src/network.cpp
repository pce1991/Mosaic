
/*
  Need a way to find the address. clients get the server address then send it their address. 
  Server adds them to a list of clients

  I want to demonstrate a couple things with networking:
  one of the players is host (turn based prob)
  How you'd build your game if there is a server that feeds data to clients
 */

int32 InitAddress(sockaddr_in *addr, u8 a, u8 b, u8 c, u8 d, uint16 port) {
    uint32 address = (a << 24) | (b << 16) | (c << 8) | d;

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(address);
    addr->sin_port = htons(port);

    return address;
}

inline uint32 MakeAddressIPv4(u8 a, u8 b, u8 c, u8 d) {
    return (a << 24) | (b << 16) | (c << 8) | d;
}


uint32 InitSocket(Socket *socketPtr, uint32 address, uint16 port, bool bindSocket = false) {
    socketPtr->handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    socketPtr->port = port;
    socketPtr->address = address;

    sockaddr_in *addr = &socketPtr->socketAddress;

    if (bindSocket) {
        addr->sin_family = AF_INET;
        addr->sin_addr.s_addr = htonl(address);
        addr->sin_port = htons(port);
    
        int32 bindSuccess = bind(socketPtr->handle, (const sockaddr*)addr, sizeof(sockaddr_in));
        if (bindSuccess != 0) {
            int32 error = WSAGetLastError();
            Print("failed to bind socket! %d", error);

            Log("Failed to bind socket for %u on port %d error: %d", address, port, error);
        }
    }

    // enable promiscuous mode. Maybe not necessary. This lets you receive all packets on network, not just to machine.
    // {
    //     DWORD dwValue = RCVALL_ON;
    //     DWORD dwBytesReturned = 0;
    //     if (WSAIoctl(socketPtr->handle, SIO_RCVALL, &dwValue, sizeof(dwValue), NULL, 0, &dwBytesReturned, NULL, NULL) == SOCKET_ERROR) {
    //         Print("Ioctl failed with error code : %d", WSAGetLastError());
    //     }
    // }

    DWORD nonBlocking = 1;
    int32 nonBlockingSuccess = ioctlsocket(socketPtr->handle, FIONBIO, &nonBlocking);

    if (nonBlockingSuccess != 0) {
        Log("Failed to set socket nonblocking for %u on port %d to non blocking", address, port);
        Print("Failed to set socket nonblocking for %u on port %d to non blocking", address, port);
    }

    return address;
}

uint32 InitSocket(Socket *socketPtr, u8 a, u8 b, u8 c, u8 d, uint16 port) {
    return InitSocket(socketPtr, MakeAddressIPv4(a, b, c, d), port);
}

int32 SendPacket(Socket *socket, uint32 address, uint16 port, void *packetData, uint32 packetSize) {
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(address);
    addr.sin_port = htons(port);
    
    return sendto(socket->handle, (char *)packetData, packetSize, 0, (sockaddr *)&addr, sizeof(sockaddr_in));
}

int32 SendPacket(Socket *socket, void *packetData, uint32 packetSize) {
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(socket->address);
    addr.sin_port = htons(socket->port);
    
    return sendto(socket->handle, (char *)packetData, packetSize, 0, (sockaddr *)&addr, sizeof(sockaddr_in));
}

int32 ReceivePacket(Socket *socket, void *buffer, uint32 bufferSize, Socket *fromSocket) {
    int32 fromSize = sizeof(sockaddr_in);
    
    return recvfrom(socket->handle, (char *)buffer, bufferSize, 0, (sockaddr *)&fromSocket->socketAddress, &fromSize);
}

uint32 GetMyAddress() {
    char hostName[64];
    int32 gotName = gethostname(hostName, sizeof(hostName));

    hostent *hostEntry = gethostbyname(hostName);
    // @PLATFORM: this is inet_pton on unix systems

    char *ipAddress = inet_ntoa((*((struct in_addr*) hostEntry->h_addr_list[0])));
    int32 address = -1;
    int32 success = InetPton(AF_INET, ipAddress, &address);
    return ntohl(address);
}

// Probably two types of packets: network and client
// I mean really we can have any number of types of packets, as long as we always receive the max size
// We'll have different updates based on if the game is a server or a client.
// This way the server can send out the chunky information and clients know what to receieve.


void ReceivePackets() {
    NetworkInfo *network = &Game->networkInfo;
    
    DynamicArrayClear(&network->packetsReceived);

    while (true) {
        ReceivedPacket packet = {};
        
        Socket fromSocket;
        int32 bytesReceived = ReceivePacket(&Game->networkInfo.receivingSocket, (u8 *)&packet.packet, sizeof(GamePacket), &fromSocket);

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

        packet.fromAddress = fromAddress;
        packet.fromPort = fromPort;

        PushBack(&network->packetsReceived, packet);
    }
}

void SendPackets() {
    NetworkInfo *info = &Game->networkInfo;

    //PushBack(&info->packetsToSend, packet);

    for (int i = 0; i < info->packetsToSend.count; i++) {
        int32 packetSize = sizeof(GamePacket);

        // @TODO: we want the packet to specify where it go.
        // The server shouldnt send all the same data to all the clients (i think)
        for (int j = 0; j < Game->networkInfo.sendingSockets.count; j++) {
            int32 bytesSent = SendPacket(&Game->networkInfo.sendingSockets[j], &info->packetsToSend[i], packetSize);
        }
    }

    DynamicArrayClear(&info->packetsToSend);
}
