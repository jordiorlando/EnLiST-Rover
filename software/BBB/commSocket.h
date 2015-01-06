#ifndef COMMSOCKET_H_
#define COMMSOCKET_H_

#include "tcpSocket.h"

class commSocket : public tcpSocket
{

    public:
        commSocket(int port);
        ~commSocket();

        int connectedfd;
        bool connected;

    private:
        void handleConnection(int socketfd);

};

#endif //COMMSOCKET_H_
