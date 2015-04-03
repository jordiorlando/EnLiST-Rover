#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include <thread>
#include <mutex>

#include <vector>
#include <iostream>
#include <cstring>      // Needed for memset

#include <sys/socket.h> // Needed for the socket functions
#include <sys/types.h>

#include <errno.h>

//addresses
#include <netinet/in.h>
#include <arpa/inet.h>

class udpSocket
{

    public:
        udpSocket(int port);
        virtual tcpSocket();

    private:
        virtual void handleConnection(int socketfd) = 0;
        void openSocket();
        void closeSocket();
        void acceptConnections();

        int port;
        int listenfd;
        bool threaded;

        bool closed;

        std::vector<std::thread> threads;
};

#endif // UDPSOCKET_H_
