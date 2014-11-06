#include "videoSocket.h"

videoSocket::videoSocket(int port) : genericSocket(port, false) {}

void videoSocket::handleConnection(int socketfd)
{

    //TODO: interpret video header data (if necessary)
    //      and send the output out of all connected clients
    //      which are stored in an int list of socketfds

    std::cout << "fd " << socketfd << std::endl;
   
    ssize_t bytes_recieved, total_bytes;
    char incoming_data_buffer[1000];
    while((bytes_recieved = read(socketfd, incoming_data_buffer, 1000)) > 0)
    {
    std::cout << bytes_recieved << " bytes recieved :" << std::endl;
    incoming_data_buffer[bytes_recieved] = '\0';
    std::cout << incoming_data_buffer << std::endl;
    }
    
    if (bytes_recieved < 0)
        std::cout << "recv()" << std::endl; //recv() error
    
    
    std::cout << "closing socket" << std::endl;
    close(socketfd);

}