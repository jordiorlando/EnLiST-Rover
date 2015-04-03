#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

//threading
#include <thread>
#include <mutex>

#include <vector>
#include <iostream>
#include <cstring>      // Needed for memset, memcpy

//socket functions
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <unistd.h>     //close

#include <errno.h>

//gethostbyname
#include <netdb.h>

//addresses
#include <netinet/in.h>
#include <arpa/inet.h>

//poll
#include <poll.h>

/* Base class for working with TCP sockets.
 *
 * Hosts:   use first constructor, implement handleConnection, send/recv/etc on given socketfd.
 * Clients: use second constructor, use send/recv/etc on hostfd.
 */
class tcpSocket
{
    public:
        tcpSocket(int port, bool threaded);
        tcpSocket(char * host, int port);
        virtual ~tcpSocket();

        int hostfd; //TODO: change this to a const access function, make data private
                    //TODO: add public host send/recv functions for clients?

    private:
        void openSocket();
        void closeSocket();
        void connectToHost(char * host);
        void startListening();
        void acceptConnections();

        virtual void handleConnection(int socketfd);

        int port;
        bool threaded;
        bool closed;
};

#endif // TCPSOCKET_H_
