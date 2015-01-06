#include "controllerInput.h"

controllerInput::controllerInput(tcpSocket & server)
{
    this->server = &server;

    pfd.fd      = this->server->hostfd;
    pfd.events  = POLLIN;
    pfd.revents = 0;
}

void controllerInput::updateValues()
{
    //wait up to 100ms
    poll(&pfd, 1, 100);

    //data ready
    if (pfd.revents & POLLIN)
    {
        ssize_t len;
        unsigned char buf[6];
        len = recv(pfd.fd, buf, 6, 0);

        std::cout << "got data " << len << " " << (unsigned int)buf[0] << " " << (unsigned int)buf[1] << " " <<
                                                  (unsigned int)buf[2] << " " << (unsigned int)buf[3] << std::endl;

        data.square   = (buf[0] >> 0) & 0x1;
        data.cross    = (buf[0] >> 1) & 0x1;
        data.circle   = (buf[0] >> 2) & 0x1;
        data.triangle = (buf[0] >> 3) & 0x1;
        data.l1       = (buf[0] >> 4) & 0x1;
        data.l2       = (buf[0] >> 5) & 0x1;
        data.r1       = (buf[0] >> 6) & 0x1;
        data.r2       = (buf[0] >> 7) & 0x1;

        data.select      = (buf[1] >> 0) & 0x1;
        data.start       = (buf[1] >> 1) & 0x1;
        data.left_stick  = (buf[1] >> 2) & 0x1;
        data.right_stick = (buf[1] >> 3) & 0x1;
        data.d_up        = (buf[1] >> 4) & 0x1;
        data.d_down      = (buf[1] >> 5) & 0x1;
        data.d_left      = (buf[1] >> 6) & 0x1;
        data.d_right     = (buf[1] >> 7) & 0x1;

        data.left_x  = buf[2] - 0x80 + (1 * (buf[2] < 0x80));
        data.left_y  = buf[3] - 0x80 + (1 * (buf[3] < 0x80));
        data.right_x = buf[4] - 0x80 + (1 * (buf[4] < 0x80));
        data.right_y = buf[5] - 0x80 + (1 * (buf[5] < 0x80));

    }
}
