#include "tcpSocket.h"

/*
 * Host constructor: opens a socket on the specified port
 * and listens for and accepts connections (on separate thread if specified).
 */
tcpSocket::tcpSocket(int port, bool threaded)
{
    std::cout << "opening port " << port << std::endl;

    this->port = port;
    this->threaded = threaded;

    openSocket();
    startListening();

    std::thread t(&tcpSocket::acceptConnections, this);
    t.detach();
}

/*
 * Client constructor: opens a socket and connects to the
 * specified host on the specified port.
 */
tcpSocket::tcpSocket(char * host, int port)
{
    std::cout << "connecting to " << host << std::endl;

    this->port = port;
    this->threaded = false;

    openSocket();
    connectToHost(host);
}

tcpSocket::~tcpSocket()
{
    closeSocket();
}

/*
 * Open the socket for all communication.
 */
void tcpSocket::openSocket()
{
    hostfd = socket(AF_INET, SOCK_STREAM, 0);
    if (hostfd < 0)
        std::cout << "socket()" << std::endl; //socket() error
    closed = false;
}

/*
 * Close the socket.
 */
void tcpSocket::closeSocket()
{
    close(hostfd);
    closed = true;
}

/*
 * Connect to a host (where host is a hostname or IPv4/6 address).
 */
void tcpSocket::connectToHost(char * host)
{
    struct sockaddr_in serv_addr;
    struct hostent * server;

    //look up host
    server = gethostbyname(host);
    if (server == NULL) {
        std::cout << "gethostbyname(): host not found" << std::endl;
        return;
    }

    //set connect address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
    serv_addr.sin_port = htons(port);

    //connect
    if (connect(hostfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        std::cout << "connect() " << std::endl; //connect() error
}

/*
 * Listen for incoming connections (become a host).
 */
void tcpSocket::startListening()
{
    struct sockaddr_in serv_addr;

    //set bind address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = htons(port);

    //bind
    if (bind(hostfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        std::cout << "bind() " << port << ": " << errno << std::endl; //bind() error

    //listen for connections
    if (listen(hostfd, 64) < 0)
        std::cout << "listen()" << std::endl; //listen() error
}

/*
 * Accept and handle incoming connections in a loop until closed.
 */
void tcpSocket::acceptConnections()
{
    int hit, connectedfd;
    socklen_t length;
    struct sockaddr_in cli_addr;

    //accept connections forever
    for(hit=1;!closed;hit++)
    {
        if ((connectedfd = accept(hostfd, (struct sockaddr *)&cli_addr, &length)) < 0)
            continue; //accept() error

        if (threaded)
        {
            std::thread t(&tcpSocket::handleConnection, this, connectedfd);
            t.detach();
        }
        else
        {
            this->handleConnection(connectedfd);
        }
    }

}

/*
 * Should be implemented by any derived classes that use
 * host operation. For clients, it does nothing.
 */
void tcpSocket::handleConnection(int socketfd) {}
