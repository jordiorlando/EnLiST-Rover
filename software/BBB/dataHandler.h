#ifndef DATAHANDLER_H_
#define DATAHANDLER_H_

#include "commSocket.h"
#include "streamSocket.h"

/*
 * Forwards controller input from a browser on streamSocket to the client on tcpSocket.
 * Forwards status input from the client on tcpSocket to all browsers on streamSocket.
 *
 * This will likely be drastically changed or possibly completely removed
 * as the project changes and matures.
 */

class dataHandler
{

    public:
        dataHandler(commSocket & client, streamSocket & browsers);
        void handleData();

    private:
        void handleClientData();
        void handleBrowserData();

        commSocket * client;
        streamSocket * browsers;
        struct pollfd pfd;

};

#endif //DATAHANDLER_H_
