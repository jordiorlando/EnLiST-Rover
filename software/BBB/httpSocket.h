#ifndef HTTPSOCKET_H_
#define HTTPSOCKET_H_

#include "genericSocket.h"

//http headers
#include <map>
#include <string>

class httpSocket : public genericSocket
{

    public:
        httpSocket(int port);

        static char * parseHTTPRequest(char * request, std::string & method, std::string & path, std::string & version, std::map<std::string, std::string> & headers);

    private:
        void handleConnection(int socketfd);


};

#endif //HTTPSOCKET_H_
