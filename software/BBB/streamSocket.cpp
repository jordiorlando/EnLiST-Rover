#include "streamSocket.h"

streamSocket::streamSocket(int port) : genericSocket(port, false) {}


/* Replaces parameter key with the accept key
 * for the WebSocket protocol.
 */
void streamSocket::acceptKey(std::string & key)
{
    //add magic string
    key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    
    //sha-1 into hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)key.c_str(), key.length(), hash);
    
    //http://www.softwarematrix.cn/blog/misc/base64-encode-decode-using-openssl-in-c
    
    //base64 encode
    EVP_ENCODE_CTX ectx;
    int size = SHA_DIGEST_LENGTH*2;
    size = size > 64 ? size : 64;
    unsigned char* out = (unsigned char*)malloc( size );
    int outlen = 0;
    int tlen = 0;
    EVP_EncodeInit( &ectx );
    EVP_EncodeUpdate( &ectx, out, &outlen, hash, SHA_DIGEST_LENGTH );
    tlen += outlen;
    EVP_EncodeFinal( &ectx, out+tlen, &outlen );
    tlen += outlen;
    //std::cout << tlen << std::endl;
    key.assign( (char*)out, tlen - 1 );
    free(out);
    
}

/* Send data to specified fd.
 */
void streamSocket::sendData(int socketfd, unsigned char * data, size_t length)
{
    
    unsigned char framedata = 0x00;
    
    //opcodes:
    //0x0: continuation
    //0x1: text (utf-8)
    //0x2: binary
    //0x9: ping (recv only)
    //0xA: pong (send only)
    //(for now, assume text 0x1)
    unsigned char opcode = 0x1;
    
    framedata |= 0x80;      //for FIN = true (multiple frame continuation not implemented)
    framedata |= opcode;
    
    //send first byte to buffer
    send(socketfd, &framedata, 1, MSG_MORE);
    
    if (length > 65535)
    {
        framedata = 127 & 0x7F;
        send(socketfd, &framedata, 1, MSG_MORE);
        
        //assuming length isn't going to use more than 4 bytes
        //because that's insanely long for one frame
        
        //framedata = (length >> 56) & 0xFF;
        //send(socketfd, &framedata, 1, MSG_MORE);
        //framedata = (length >> 48) & 0xFF;
        //send(socketfd, &framedata, 1, MSG_MORE);
        //framedata = (length >> 40) & 0xFF;
        //send(socketfd, &framedata, 1, MSG_MORE);
        //framedata = (length >> 32) & 0xFF;
        //send(socketfd, &framedata, 1, MSG_MORE);
        framedata = (length >> 24) & 0xFF;
        send(socketfd, &framedata, 1, MSG_MORE);
        framedata = (length >> 16) & 0xFF;
        send(socketfd, &framedata, 1, MSG_MORE);
        framedata = (length >> 8) & 0xFF;
        send(socketfd, &framedata, 1, MSG_MORE);
        framedata = (length) & 0xFF;
        send(socketfd, &framedata, 1, MSG_MORE);
    }
    else if (length > 125)
    {
        framedata = 126 & 0x7F;
        send(socketfd, &framedata, 1, MSG_MORE);
        
        framedata = (length >> 8) & 0xFF;
        send(socketfd, &framedata, 1, MSG_MORE);
        framedata = (length) & 0xFF;
        send(socketfd, &framedata, 1, MSG_MORE);
    }
    else
    {
        framedata = length & 0x7F;
        send(socketfd, &framedata, 1, MSG_MORE);
    }

    
    //send the actual data
    send(socketfd, data, length, 0);
}

/* Receive data from specified fd.
 * The opcode of the data recieved is set in opcode.
 */
std::vector<unsigned char> streamSocket::recvData(int socketfd, unsigned char & opcode)
{

    std::vector<unsigned char> data(8);
    
    size_t bytes_received = recv(socketfd, data.data(), 2, 0);
    
    if (bytes_received < 2)
    {
        //something went wrong: 
        //read() error (<0), closed connection (EOF=0), or invalid data (!=2)
        opcode = 0x08;
        data.clear();
        return data;
    }
    
    //if fin is 0, there are more frames to receieve (not implemented)
    bool fin = data[0] >> 7; 
    
    //the mask flag indicates if the message is XOR encryped (it should be 1)
    bool masked = data[1] >> 7;    
    
    if (!masked || !fin)
    {
        //not allowed
        opcode = 0x08;
        data.clear();
        return data;
    }
    
    //opcodes:
    //0x0: continuation
    //0x1: text (utf-8)
    //0x2: binary
    //0x8: connection close
    //0x9: ping (recv only)
    //0xA: pong (send only)
    opcode = data[0] & 0x0f;
    
    unsigned char len = data[1] & 0x7f;
    uint64_t length;
    
    if (len == 126) //16 bit length
    {
        bytes_received = recv(socketfd, data.data(), 2, 0);
        length = ((uint64_t)data[0] << 8) | 
                 ((uint64_t)data[1]);
    }
    else if (len == 127) //64 bit length
    {
        bytes_received = recv(socketfd, data.data(), 2, 0);
        length = ((uint64_t)data[0] << 56) | 
                 ((uint64_t)data[1] << 48) |
                 ((uint64_t)data[2] << 40) |
                 ((uint64_t)data[3] << 32) |
                 ((uint64_t)data[4] << 24) |
                 ((uint64_t)data[5] << 16) |
                 ((uint64_t)data[6] << 8) |
                 ((uint64_t)data[7]);
    }
    else //8 bit length
    {
        length = len;
    }
    
    if (length > 102040)
    {
        //data too large
        opcode = 0x08;
        data.clear();
        return data;
    }
    
    std::vector<unsigned char> mask(4);
    bytes_received = recv(socketfd, mask.data(), 4, 0);
    
    data.resize(length);
    bytes_received = recv(socketfd, data.data(), data.size(), 0);
    
    //unmask the data
    for (size_t i = 0; i<length; i++)
    {
        data[i] = data[i] ^ mask[i % 4];
    }
    
    return data;
    
}

/* Calls poll on the fd list.
 */
void streamSocket::pollFDList()
{
    poll(fdlist.data(), fdlist.size(), 1000);
}

/* Closes the connection with the specified fd and
 * removes the fd from the list.
 */
void streamSocket::closeSocket(int socketfd)
{
    for (size_t i=0; i<fdlist.size(); i++)
    {
        if (fdlist[i].fd == socketfd)
        {
            fdlist.erase(fdlist.begin() + i);
            break;
        }
    }
    
    close(socketfd);
}

void streamSocket::handleConnection(int socketfd)
{

    //recieve data
    ssize_t bytes_received, total_bytes;
    char * incoming_data_buffer = (char *)malloc(1000 * sizeof(char));
    //char incoming_data_buffer[1000];
    //std::vector<char> incoming_data_buffer(500);
    
    bytes_received = read(socketfd, incoming_data_buffer, 999);
    incoming_data_buffer[bytes_received] = '\0';
    
    std::cout << bytes_received << std::endl;
    
    //parse HTTP request data
    std::string method, path, version;
    std::map<std::string, std::string> headers;
        
    httpSocket::parseHTTPRequest(incoming_data_buffer, method, path, version, headers);   
    
    free(incoming_data_buffer);
    
    //send back accept response
    auto it = headers.find("Sec-WebSocket-Key");
    if (it == headers.end())
    {
        //no key header found
        close(socketfd);
        return;
    }
    
    std::string key = it->second;
    acceptKey(key);

    std::string response;
    response += "HTTP/1.1 101 Switching Protocols\r\n";
    response += "Upgrade: websocket\r\n";
    response += "Connection: Upgrade\r\n";
    response += "Sec-WebSocket-Accept: ";
    response += key;
    response += "\r\n\r\n";

    ssize_t bytes_sent;
    bytes_sent = write(socketfd, response.c_str(), response.length());
    
    //socket is now connected, save fd to list
    struct pollfd pfd;
    pfd.fd = socketfd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    
    fdlist.push_back(pfd);
    
    
}

