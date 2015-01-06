#include "videoSocket.h"

videoSocket::videoSocket(int port, streamSocket & ss) :
    tcpSocket(port, false), streamOut(&ss) {}

void videoSocket::handleConnection(int socketfd)
{

    ssize_t bytes_received, total_bytes;
    unsigned char * incoming_data_buffer = (unsigned char*)malloc(2001);

    //datatype byte (video feed)
    incoming_data_buffer[0] = 0x1;

    while((bytes_received = read(socketfd, incoming_data_buffer + 1, 2000)) > 0)
    {
        streamOut->sendToAll(incoming_data_buffer, bytes_received + 1);
    }

    std::cout << "0 end" << std::endl;

    if (bytes_received < 0)
        std::cout << "recv()" << std::endl; //recv() error

    free(incoming_data_buffer);

    close(socketfd);

}
