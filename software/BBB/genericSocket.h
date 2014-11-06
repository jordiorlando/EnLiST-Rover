#ifndef GENERICSOCKET_H_
#define GENERICSOCKET_H_

#include <thread>
#include <mutex>

#include <vector>
#include <iostream>
#include <cstring>      // Needed for memset

#include <sys/socket.h> // Needed for the socket functions
#include <sys/types.h>

//addresses
#include <netinet/in.h>
#include <arpa/inet.h>

class genericSocket
{

    public:
        genericSocket(int port, bool threaded);
        virtual ~genericSocket();
        
    private:
        virtual void handleConnection(int socketfd) = 0;
        void openSocket();
        void closeSocket();
        void acceptConnections();
        
        int port;       
        int listenfd;   
        bool threaded;
        
        std::vector<std::thread> threads;
};

#endif // GENERICSOCKET_H_