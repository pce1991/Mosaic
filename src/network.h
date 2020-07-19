
struct Socket {
    int32 handle;

    uint32 address[4];
    int32 port;

    sockaddr_in socketAddress;
};
