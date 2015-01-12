#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include "tcpSocket.h"

//http headers
#include <map>
#include <string>

//sha-1
#include <openssl/sha.h>

//base64
#include <openssl/bio.h>
#include <openssl/evp.h>

//ppoll
#include <poll.h>

class streamSocket : public tcpSocket
{

    public:
        streamSocket(int port);
        void pollFDList(int timeout);
        void closeSocket(int socketfd);

        void sendToAll(unsigned char * data, size_t length);
        static void sendData(int socketfd, unsigned char * data, size_t length, uint8_t opcode = 0x02);
        static std::vector<unsigned char> recvData(int socketfd, unsigned char & opcode);

    static char * parseHTTPRequest(char * request, std::string & method, std::string & path,
                                       std::string & version, std::map<std::string, std::string> & headers);

        std::vector<struct pollfd> fdlist;
        std::mutex fd_mutex;

    private:
        void handleConnection(int socketfd);
        static void acceptKey(std::string & key);

};

#endif //STREAMSOCKET_H_
