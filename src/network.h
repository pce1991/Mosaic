
struct Socket {
    // This is a handle to the socket in the win32 layer
    int32 handle;

    uint32 address;
    uint16 port;

    sockaddr_in socketAddress;
};


// @TODO: This really seems like stuff that needs to be defined on the game side
enum GamePacketType {
     GamePacketType_Invalid,
     GamePacketType_Ping,
     GamePacketType_String,

     GamePacketType_Input,
     
     
     GamePacketType_Message,
     GamePacketType_Username,

     GamePacketType_Pong,
};

// @NOTE: this is the specification for the packet that gets sent and received.
// We do it this way because we cant guarantee the order that packets come in,
// so it isn't posibble to just send one header packet that is (type size) and know that
// the next packet is the payload.
struct GamePacket {
    // This is an ID which we require every packed to send. The server ignores any data it
    // receives that doesnt begin with this id.
    int32 id;
    GamePacketType type;
    int32 frame;

    // Sometimes we want to send over a chunk of data that has multiple parts.
    // The ID tells us which it belongs to, the index tells the receiver how to reconstruct it. 
    int8 partID;
    int8 partIndex;

    // use memcpy to fill this data
    u8 data[256];
};

// Lets say we want to send an arbitrary string. We cant know it fits inside our packet size. I think we'd
// instead break it up into multiple packets, attach an id which tells the server how to construct it. 

struct ReceivedPacket {
    // @NOTE: rather than make sender responsible for their address,
    // whoever is looking receiving packets writes out the address.
    // This saves sending redundant data.
    uint32 fromAddress;
    uint16 fromPort;

    GamePacket packet;
};


struct NetworkInfo {
    // List of addresses to contact.
    // For a client they'd only have the severer prob and be like "tell the server to message user ID" and
    // the server would look that up the info for that player and then message them.
    // For pier-to-pier there'd still be a server everyone connects to simply to get the list of connected users

    // @TODO: specify the ip address of the server in a file. Need to be able to run instance of engine
    // as a server.

    char *serverIPString;

    // @NOTE: this comes from the config file. Its use is so you can open multiple
    // instances of the game on one local machine and allow them each communicate
    // on different ports. In some cases it might be preferable to simple using 0 port
    // and letting OS pick one for us.
    uint16 configPort;

    Socket socket;

    DynamicArray<ReceivedPacket> packetsReceived;
    DynamicArray<GamePacket> packetsToSend;
};
