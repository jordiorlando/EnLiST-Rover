#include <iostream>

//usleep
#include <unistd.h>

//tcp
#include "commSocket.h"     //for connection with client
#include "videoSocket.h"    //for receiving video input feed
#include "streamSocket.h"   //for communicating with browser clients via WebSocket

//data rerouting
#include "dataHandler.h"

//signal handler
#include <signal.h>

/*
 * Main application intended to run on the server.
 * -Handles WebSocket connections, client connection.
 * -Routes incoming video data to all WebSocket connections.
 * -Routes incoming sensor data to all WebSocket connections.
 * -Routes incoming controller data to the client.
 */

static int running = 1;
void sig_handler(int sig)
{
    running = 0;
}

int main()
{
    signal(SIGINT, sig_handler);
    std::cout << "starting..." << std::endl;

    //open socket for client
    commSocket client(2222);

    //create WebSocket handler class
    streamSocket ss(5556);

    //create video stream handler class
    videoSocket vs(5000, ss);

    //create data i/o handler class
    dataHandler dh(client, ss);

    //main loop
    while(running)
    {
        dh.handleData();
        //usleep(100 * 1000);
    }

    std::cout << "stopping..." << std::endl;
    return 0;
}
