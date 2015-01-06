#include "commSocket.h"

commSocket::commSocket(int port) : tcpSocket(port, false), connected(false) {}

commSocket::~commSocket()
{
    close(connectedfd);
}

void commSocket::handleConnection(int socketfd)
{

    /*
     * TODO: if this class is used going forward, use some way to track
     * the connectivity status of the client and verify that it is the
     * client connecting.
     */

    std::cout << "client connected" << std::endl;

    connectedfd = socketfd;
    connected = true;

}
