#include "genericSocket.h"
#include <errno.h>

genericSocket::genericSocket(int port, bool threaded)
{
    std::cout << "opening port " << port << std::endl;

    this->port = port;
    this->threaded = threaded;
    
    openSocket();
    
    std::thread t(&genericSocket::acceptConnections, this);
    t.detach();
}

genericSocket::~genericSocket()
{
    closeSocket();
}

void genericSocket::openSocket()
{
    struct sockaddr_in serv_addr;
    
    //open socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) 
        std::cout << "socket()" << std::endl; //socket() error
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = htons(port);
    
    //bind socket
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        std::cout << "bind() " << port << ": " << errno << std::endl; //bind() error
    
    //listen for connections
    if (listen(listenfd, 64) < 0)
        std::cout << "listen()" << std::endl; //listen() error

}

void genericSocket::closeSocket()
{
    close(listenfd);
}

void genericSocket::acceptConnections()
{
    int hit, socketfd;
    socklen_t length;
    struct sockaddr_in cli_addr;
    
    //accept connections forever
    for(hit=1;;hit++)
    {
        if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
            continue; //accept() error
            
        if (threaded)
        {
            std::thread t(&genericSocket::handleConnection, this, socketfd);
            t.detach();
        }
        else 
        {
            this->handleConnection(socketfd);
        }
    }

}