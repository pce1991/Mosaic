
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
// SendPacket()
// ReceivePacket()
//    We need to know the maxsize of a packet. I dont imagine its a good idea to try to send over mini packets
//    that specify the type and size and then use that to retrieve the whole packet?

int32 ReceivePacket(Socket *socket, void *buffer, uint32 bufferSize, Socket *fromSocket) {
    int32 fromSize = sizeof(sockaddr_in);
    
    return recvfrom(socket->handle, (char *)buffer, bufferSize, 0, (sockaddr *)&fromSocket->socketAddress, &fromSize);
}


struct GamePacket {
    int32 frame;
};
// Probably two types of packets: network and client
// I mean really we can have any number of types of packets, as long as we always receive the max size

struct NetworkInfo {
    // List of addresses to contact.
    // For a client they'd only have the severer prob and be like "tell the server to message user ID" and
    // the server would look that up the info for that player and then message them.
    // 

    // buffer that gets filled with packets
    int32 packetCapacity;
    int32 packetCount;
    GamePacket *packets;
};

