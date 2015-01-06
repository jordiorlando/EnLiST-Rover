#include "httpSocket.h"

#include <cstring>
#include <sstream>
#include <stdlib.h>

#include <iostream>
#include <fstream>

httpSocket::httpSocket(int port) : genericSocket(port, true) {}

/*
 * Parse http request, puts useful values in the parameter references,
 * returns pointer to the start of the request body.
 */
char * httpSocket::parseHTTPRequest(char * request, std::string & method, std::string & path, std::string & version, std::map<std::string, std::string> & headers)
{
    int i = 0;
    char * start = request;
    bool data = false;

    //parse request line
    while (i < 1000)
    {
        if (request[i] == ' ' || request[i] == '\t' || request[i] == '\r' || request[i] == '\n')
        {
            //whitespace
            if (data)
            {
                //assign to appropriate string
                request[i] = '\0';
                if (method.empty())
                    method = start;
                else if (path.empty())
                    path = start;
                else if (version.empty())
                    version = start;

                data = false;
            }

            //end of request line
            if (request[i] == '\r' || request[i] == '\n')
            {
                //move to next char
                i++;

                //skip \n if necessary
                if (i < (1000 - 1) && request[i + 1] == '\n')
                    i++;
                break;
            }
        }
        else
        {
            //actual data (not whitespace)
            if (!data)
                start = &request[i];
            data = true;
        }
        i++;
    }

    std::string currentHeader;

    //parse headers
    //TODO: remove hardcoded buffer length
    while (i < 1000)
    {

        if (request[i] == '\r' || request[i] == '\n')
        {
            //value found
            if (data)
            {
                request[i] = '\0';
                headers[currentHeader] += start;
                data = false;

                //skip \n if necessary
                if (i < (1000 - 1) && request[i + 1] == '\n')
                    i++;
            }
            else
            {
                //move to next line (beginning of data)
                //this is questionable code...
                start = &request[i + 1];
                if (request[i] == '\r' && i < (1000 - 2))
                    start++;
                break;
            }
        }
        else if (request[i] == ' ' || request[i] == '\t')
        {
            //ignore whitespace
        }
        else if (request[i] == ':')
        {
            //header found
            if (data)
            {
                //ignore if no whitespace after
                if (i < (1000 - 1) && (request[i + 1] == ' ' || request[i + 1] == '\t'))
                {
                    request[i] = '\0';
                    currentHeader = start;
                    data = false;
                }
            }
        }
        else
        {
            //actual data
            if (!data)
            {
                start = &request[i];
                data = true;
            }
        }

        i++;
    }

    return start;
}

void httpSocket::handleConnection(int socketfd)
{

    ssize_t bytes_received;
    char incoming_data_buffer[1000];

    bytes_received = read(socketfd, incoming_data_buffer, 1000);
    incoming_data_buffer[bytes_received] = '\0';

    if (bytes_received <= 0)
        std::cout << "recv()" << std::endl; //recv() error or empty request

    //parse HTTP request data
    std::string method, path, version;
    std::map<std::string, std::string> headers;
    parseHTTPRequest(incoming_data_buffer, method, path, version, headers);

    if (method == "GET" || method == "get")
    {

    }
    else if (method == "POST" || method == "post")
    {

    }

    /*

    std::string response;
    response += "HTTP/1.1 101 Switching Protocols\r\n";
    response += "Upgrade: websocket\r\n";
    response += "Connection: Upgrade\r\n";
    response += "Sec-WebSocket-Accept: ";
    response += key;
    response += "\r\n\r\n";

    */

    //ssize_t bytes_sent;
    //bytes_sent = write(socketfd, response.c_str(), response.length());

    std::streampos size;
    char * memblock;

    std::ifstream file ("testpage.html", std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
    size = file.tellg();
    memblock = new char [size];
    file.seekg (0, std::ios::beg);
    file.read (memblock, size);
    file.close();

    size_t bytes_sent = write(socketfd, memblock, size);

    delete[] memblock;
    }

    close(socketfd);

}

/*
void httpSocket::handleGET(int socketfd)
{

}

void httpSocket::handlePOST(int socketfd)
{

}
*/
