#ifndef VIDEOSOCKET_H_
#define VIDEOSOCKET_H_

#include "genericSocket.h"

class videoSocket : public genericSocket
{

    public:
        videoSocket(int port);
        
    private:
        void handleConnection(int socketfd);


};

#endif //VIDEOSOCKET_H_