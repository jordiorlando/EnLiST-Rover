#ifndef VIDEOSOCKET_H_
#define VIDEOSOCKET_H_

#include "tcpSocket.h"
#include "streamSocket.h"

class videoSocket : public tcpSocket
{

    public:
        videoSocket(int port, streamSocket & ss);

    private:
        void handleConnection(int socketfd);
        streamSocket * streamOut;

};

#endif //VIDEOSOCKET_H_
