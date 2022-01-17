
// This is my IP address on the local network.
// Only machines on my local network are able to communicate
// with this address.
// open up command prompt and type ipconfig to find out your machine's local IP address
const uint32 ServerAddress = MakeAddressIPv4(192, 168, 1, 179);
const uint16 ServerPort = 30000;

// @NOTE TO STUDENTS: look up the hash function in the engine!
const int32 PacketID = Hash("NetworkSimple");

struct ClientInfo {
    uint32 address;
    uint16 port;

    real32 lastPingTime;
};

struct ServerData {
    // @NOTE: we could have a buffer of these if we wanted multiple clients.
    ClientInfo clients[2];
};

struct ClientData {
    real32 lastPingTimeFromServer;
    bool connected;
};

struct MyData {
    ServerData serverData;

    ClientData clientData;
};

// Client mode and a Server mode
// The client is going to send pings to the server
// The server is going to listen for pings and then send pings to all the clients
// The client knows the servers address, but the server doesnt know who the client is yet

MyData *Data = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));
    
    Data = (MyData *)Game->myData;

    NetworkInfo *network = &Game->networkInfo;

    // Create sockets that we'll use to communicate.

    // This means that this code is being executed by server.exe
    if (IS_SERVER) {
        InitSocket(&network->socket, GetMyAddress(), ServerPort, true);
    }
    else {
        // If we don't care what port we're on we can pass in 0 and it'll pick one
        // for us. This can be useful for doing networking locally where we want
        // to have multiple game instances open and have the server communicate with each.
        InitSocket(&network->socket, GetMyAddress(), 0, true);
    }
}

void ClientUpdate() {
    NetworkInfo *network = &Game->networkInfo;

    ClientData *client = &Data->clientData;

    GamePacket packet = {};
    packet.id = PacketID;
    packet.type = GamePacketType_Ping;
    packet.frame = Game->frame;

    // If you dont know what memcpy is, you need to look it up ASAP
    memcpy(packet.data, &Game->time, sizeof(real32));

    // Honestly we could just send the packet directly, but I want to showcase
    // that we have a buffer to accumulate multiple packets and then send
    // over in one pass.
    PushBack(&network->packetsToSend, packet);
    

    // Here we send the packets where we want to.
    for (int i = 0; i < network->packetsToSend.count; i++) {
        GamePacket *p = &network->packetsToSend[i];
        uint32 bytesSent = SendPacket(&network->socket, ServerAddress, ServerPort, p, sizeof(GamePacket));
        if (bytesSent != sizeof(GamePacket)) {
            Print("Failed to send %d bytes, sent %d instead", sizeof(GamePacket), bytesSent);
        }
    }

    // TODO: We should probably check to make sure the packet id is from our game.
    if (network->packetsReceived.count > 0) {
        client->lastPingTimeFromServer = Game->time;
        client->connected = true;
    }

    if (client->connected) {
        DrawTextScreen(&Game->serifFont, V2(0.5f, 0.1f), 0.02f, V4(1), true, "CONNECTED TO SERVER!");
    }
    else {
        DrawTextScreen(&Game->serifFont, V2(0.5f, 0.1f), 0.02f, V4(1, 0, 0, 1), true, "NO CONNECTION...");        
    }
    
    real32 timeSincePing = Game->time - client->lastPingTimeFromServer;

    if (timeSincePing > 1.0f) {
        client->connected = false;
    }
    
    DrawTextScreen(&Game->serifFont, V2(0.5f, 0.2f), 0.02f, V4(1, 0, 0, 1), true, "Last Ping Time %.2f", timeSincePing);
}

void ServerUpdate() {
    NetworkInfo *network = &Game->networkInfo;
    
    ServerData *server = &Data->serverData;
    
    for (int i = 0; i < network->packetsReceived.count; i++) {
        ReceivedPacket *r = &network->packetsReceived[i];
        GamePacket *p = &r->packet;

        // Remember, the first 4 bytes of our packet tell have the id,
        // so if our packet doesnt have those same bytes set then this
        // is a message someone sent to our server, but it wasnt sent
        // from our game.
        if (p->id != PacketID) {
            continue;
        }

        ClientInfo *foundClient = NULL;
        
        for (int j = 0; j < 2; j++) {
            ClientInfo *client = &server->clients[j];

            // First check to see if we already have a connection,
            // if we don't then we'll check to see if we should create one,
            // otherwise it's just the case that this packet isnt coming from this client.
            if (r->fromAddress == client->address && r->fromPort == client->port) {
                foundClient = client;
                break;
            }
            else if (client->address == 0) {
                client->address = r->fromAddress;
                client->port = r->fromPort;

                foundClient = client;
                break;
            }
        }

        if (foundClient == NULL) {
            continue;
        }

        // This is the time that THEY sent the ping, not when we received it.
        // If this seems strange to you make sure you look up pointer dereferencing
        // and pointer arithmetic. Remember, our data is just an array of bytes.
        // Since we know we're only sending pings we can just assume the first
        // 4 bytes of the data are the time (look at ClientUpdate()).
        foundClient->lastPingTime = *((real32 *)p->data);
    }

    
    if (server->clients[0].address != 0) {
        GamePacket packet = {};
        packet.id = PacketID;
        packet.type = GamePacketType_Ping;
        packet.frame = Game->frame;

        // If you dont know what memcpy is, you need to look it up ASAP
        memcpy(packet.data, &Game->time, sizeof(real32));

        // Honestly we could just send the packet directly, but I want to showcase
        // that we have a buffer to accumulate multiple packets and then send
        // over in one pass.
        PushBack(&network->packetsToSend, packet);
    

        // Here we send the packets where we want to.
        // @NOTE: our server is very simple: it assumes only one client,
        // but if we had multiple clients we might want to send this data to
        // all of them, or maybe only send some data to some of them,
        // in which case we might want something more sophisticated than just
        // one buffer of GamePackets
        for (int i = 0; i < network->packetsToSend.count; i++) {
            GamePacket *p = &network->packetsToSend[i];

            for (int j = 0; j < 2; j++) {
                ClientInfo *client = &server->clients[j];
                uint32 bytesSent = SendPacket(&network->socket, client->address, client->port, p, sizeof(GamePacket));

                if (bytesSent != sizeof(GamePacket)) {
                    Print("Failed to send %d bytes, sent %d instead", sizeof(GamePacket), bytesSent);
                }
            }
        }
    }
}

void MyGameUpdate() {
    NetworkInfo *network = &Game->networkInfo;

    // DynamicArrays are like Lists in C# in case you didnt know.
    // We want to clear everything from the array so we only ever send packets
    // that we created this frame. 
    DynamicArrayClear(&network->packetsToSend);

    // Whether client or server we always want to receive packets.
    ReceivePackets(&network->socket);

    if (IS_SERVER) {
        ServerUpdate();
    }
    else {
        ClientUpdate();
    }
}
