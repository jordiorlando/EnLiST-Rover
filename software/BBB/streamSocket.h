#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include "genericSocket.h"

//for parseHTTPRequest
#include "httpSocket.h"

//sha-1
#include <openssl/sha.h>

//base64
#include <openssl/bio.h>
#include <openssl/evp.h>

//ppoll
#include <poll.h>

class streamSocket : public genericSocket
{

    public:
        streamSocket(int port);
        void pollFDList();
        void closeSocket(int socketfd);
        static void sendData(int socketfd, unsigned char * data, size_t length);
        static std::vector<unsigned char> recvData(int socketfd, unsigned char & opcode);
        
        std::vector<struct pollfd> fdlist;
        std::mutex fd_mutex;
        
    private:
        void handleConnection(int socketfd);
        static void acceptKey(std::string & key);
        
};

#endif //STREAMSOCKET_H_