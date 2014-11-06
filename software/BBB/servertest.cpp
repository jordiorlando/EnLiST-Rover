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

int main()
{   
    std::cout << "starting..." << std::endl;

    httpSocket hs(80);
    
    //videoSocket vs(5556);
    
    streamSocket ss(5556);
    
    i2cManager i2cM;
    i2cDevice test_dev(i2cM, 0x42);
    i2cDevice temp(i2cM, 0x5A);
    
    std::vector<unsigned char> on;
    on.push_back(0x02);
    std::vector<unsigned char> off;
    off.push_back(0x01);
    
    test_dev.i2cWrite(0, on);
    
    std::string send_data("acknowledgement.");
    
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
}


