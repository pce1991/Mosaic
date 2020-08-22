
const uint32 ServerAddress = MakeAddressIPv4(192, 168, 1, 35);
const uint16 Port = 30000;

const uint32 PacketID = Hash("Go Multiplayer");

// This is a peer to peer connection.

enum Stone {
    Stone_Empty,
    Stone_White,
    Stone_Black,
};

struct Go {
    bool foundConnection;
    bool connectingToHost;
    
    bool isHost;

    char peerAddressString[32];
    uint32 peerAddress;

    int32 captureCount[2];
    
    Stone board[361];
};

Go *myData = NULL;

void MyInit() {
    
}

void MyUpdate() {

    if (!myData->foundConnection) {

        
        // If we get a packet that means someone connected as host
        // If we send a packet we need to wait until we hear back.

        myData->foundConnection = true;
    }
    else {
        if (myData->isHost) {
            // see if peer made any moves
        }
        else {
            // get any moves from host.
            // When we make a move we dont have to wait on the host except for updates like pieces removed.
        }
    }

    // Draw the board
    // Display captured stone count
    // Button to pass
}
