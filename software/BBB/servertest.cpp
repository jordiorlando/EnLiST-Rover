#include <iostream>

//sleep
#include <chrono>
#include <thread>

//stream (number to string)
#include <sstream>

//tcp
#include "httpSocket.h"
#include "videoSocket.h"
#include "streamSocket.h"

//i2c
#include "i2cDevice.h"
#include "i2cManager.h"

//i2c devices
#include "IRTempSensor.h"
#include "MotorController.h"
#include "AmbientTempSensor.h"

//current time (for printing, nothing special)
#include <ctime>

//signal handler
#include <signal.h>

static int running = 1;
void sig_handler(int sig)
{
    running = 0;
}

int main()
{
    signal(SIGINT, sig_handler);
    std::cout << "starting..." << std::endl;

    //start listening for web connections, serve a page
    //soon to be deprecated
    httpSocket hs(80);

    //listen for websocket connections, connect them
    streamSocket ss(5556);

    //listen for video stream connections, forward to websockets
    videoSocket vs(5000, &ss);

    i2cManager i2cM;

    AmbientTempSensor ats(i2cM);
    std::cout << ats.CtoF(ats.getAmbientTemp()) << std::endl;

    //MotorController motor0(i2cM, 42);
    //MotorController motor1(i2cM, 43);

    //LCD display(i2cM);
    //IRTempSensor temp(i2cM);
    //i2cDevice rgbcolor(i2cM, 0x29);

    //display.init();

    /*
    int i=0;
    std::string out;
    time_t t;
    struct tm * now;
    while (i++ < 20)
    {
        display.setCursor(0, 0);

        t = time(0);
        now = localtime(&t);

        std::ostringstream convert;
        convert << (((now->tm_hour - 1) % 12) + 1) << ":" << now->tm_min << ":" << now->tm_sec << "    ";
        std::cout << convert.str() << std::endl;
        display.print(convert.str());
        usleep(1000 * 500);
    }

    display.print("Ryan's smart watch");
    display.setCursor(0, 2);
    display.print("Heart rate: ");
    display.setCursor(0, 3);
    display.print("around 60 prolly");
    */

    //std::cout << temp.getAmbientTemp() << std::endl;

    while(running)
    {
        ss.pollFDList();

        for (size_t i=0; i<ss.fdlist.size(); i++)
        {
            if (ss.fdlist[i].revents & POLLERR)
            {
                ss.closeSocket(ss.fdlist[i].fd);
                std::cout << "disconnected (error). " << ss.fdlist.size() << std::endl;
            }
            else if (ss.fdlist[i].revents & POLLIN)
            {
                //for now, just print the data and send a generic message
                unsigned char opcode = 0x0;
                size_t offset = 1;
                std::vector<unsigned char> d = ss.recvData(ss.fdlist[i].fd, opcode);
                std::string s(d.begin(), d.end());
                if (opcode == 0x08)
                {
                    ss.closeSocket(ss.fdlist[i].fd);
                    std::cout << "disconnected. " << ss.fdlist.size() << std::endl;
                    continue;
                }
                else
                {
                    if (s == "run")
                    {
                        //motor0.setSpeed(255);
                        //motor1.setSpeed(255);
                    }
                    else if (s == "stop")
                    {
                        //motor0.setSpeed(0);
                        //motor1.setSpeed(0);
                    }
                    else if (s == "lampon")
                    {
                        //motor0.setLamp(255);
                        //motor1.setLamp(255);
                    }
                    else if (s == "lampoff")
                    {
                        //motor0.setLamp(0);
                        //motor1.setLamp(0);
                    }
                    else if (s == "backwards")
                    {
                        //motor0.setSpeed(-255);
                        //motor1.setSpeed(-255);
                    }
                    else if (s[0] == 'l')
                    {
                        s.erase(0, 1);
                        //motor0.setLamp(std::stoi(s));
                        //motor1.setLamp(std::stoi(s));
                    }
                    else
                    {
                        //motor0.setSpeed(std::stoi(s));
                        //motor1.setSpeed(std::stoi(s));
                    }
                }
            }
        }
    }

    std::cout << "closing..." << std::endl;

    return 0;

    //std::string send_data("acknowledgement.");


    /*
    while(1) {



        ss.pollFDList();

        for (size_t i=0; i<ss.fdlist.size(); i++)
        {
            if (ss.fdlist[i].revents & POLLERR)
            {
                ss.closeSocket(ss.fdlist[i].fd);
                std::cout << "disconnected (error). " << ss.fdlist.size() << std::endl;
            }
            else if (ss.fdlist[i].revents & POLLIN)
            {

                //for now, just print the data and send a generic message
                unsigned char opcode = 0x0;
                std::vector<unsigned char> d = ss.recvData(ss.fdlist[i].fd, opcode);
                if (opcode == 0x08)
                {
                    ss.closeSocket(ss.fdlist[i].fd);
                    std::cout << "disconnected. " << ss.fdlist.size() << std::endl;
                    continue;
                }
                std::string str(d.begin(), d.end());
                if (str == "brake")
                {
                    send_data = "braking...";
                    test_dev.i2cWrite(0, off);
                }
                else if (str == "go")
                {
                    send_data = "moving...";
                    test_dev.i2cWrite(0, on);
                }
                else if (str == "read")
                {
                    std::vector<unsigned char> datain = test_dev.i2cRead(0, 6);
                    std::string str2(datain.begin(),datain.end());
                    send_data = str2;
                }
                else if (str == "temp")
                {
                    std::vector<unsigned char> datain = temp.i2cRead(0x007, 2);
                    std::ostringstream convert;
                    convert << ((double)(datain[1] << 8 | datain[0]) * 0.02 - 273.15) << " deg";
                    send_data = convert.str();
                }
                else
                {
                    send_data = "acknowledgement.";
                }
                std::cout << str << std::endl;
                ss.sendData(ss.fdlist[i].fd, (unsigned char *)send_data.data(), send_data.size());
            }
        }

        //std::chrono::milliseconds dura( 1000 );
        //std::this_thread::sleep_for( dura );
    }

    return 0;

    */
}
