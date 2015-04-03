#ifndef CONTROLLERINPUT_H_
#define CONTROLLERINPUT_H_

#include "tcpSocket.h"

/*
 * Recieves controller data from the server and parses it into useful data.
 *
 * This will likely be drastically changed or possibly completely removed
 * as the project changes and matures.
 */

class controllerInput
{

    public:
        controllerInput(tcpSocket & server);
        void updateValues();

        struct controllerData
        {
            float left_x;
            float left_y;
            float right_x;
            float right_y;

            // int8_t left_x;
            // int8_t left_y;
            // int8_t right_x;
            // int8_t right_y;

            bool square;
            bool cross;
            bool circle;
            bool triangle;
            bool l1;
            bool l2;
            bool r1;
            bool r2;
            bool select;
            bool start;
            bool left_stick;
            bool right_stick;
            bool d_up;
            bool d_down;
            bool d_left;
            bool d_right;
        };

        controllerData data;

    private:
        tcpSocket * server;
        struct pollfd pfd;

};

#endif //CONTROLLERINPUT_H_
