#include "dataHandler.h"

dataHandler::dataHandler(commSocket & client, streamSocket & browsers)
{
    this->client   = &client;
    this->browsers = &browsers;

    //pfd.fd      = this->client->connectedfd;
    pfd.events  = POLLIN;
    pfd.revents = 0;
}

void dataHandler::handleData()
{
    //update fd of the client connection
    pfd.fd = this->client->connectedfd;

    handleClientData();
    handleBrowserData();
}

void dataHandler::handleClientData()
{
    if (!client->connected) return;

    //don't wait
    if (poll(&pfd, 1, 0) == POLLNVAL)
    {
      client->connected = false;
      return;
    }

    //data ready
    if (pfd.revents & POLLIN)
    {
        unsigned char buf[1001];
        ssize_t len;
        len = recv(pfd.fd, buf + 1, 1000, 0);

        //send datatype byte
        buf[0] = 0x0;
        browsers->sendToAll(buf, len + 1);
    }
}

void dataHandler::handleBrowserData()
{
    //don't wait
    browsers->pollFDList(100);

    //data ready on any websocket
    for (size_t i=0; i<browsers->fdlist.size(); i++)
    {
        if (browsers->fdlist[i].revents & POLLERR)
        {
            browsers->closeSocket(browsers->fdlist[i].fd);
        }
        else if (browsers->fdlist[i].revents & POLLIN)
        {
            unsigned char opcode = 0x0;
            std::vector<unsigned char> data = browsers->recvData(browsers->fdlist[i].fd, opcode);

            if (opcode == 0x08) //disconnect
            {
                browsers->closeSocket(browsers->fdlist[i].fd);
            }
            else
            {
                if (!client->connected) continue;

                //forward the data
                send(pfd.fd, data.data(), data.size(), 0);
            }
        }
    }
}
