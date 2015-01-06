#include <iostream>

//usleep
#include <unistd.h>

//tcp
#include "tcpSocket.h"      //for connecting to the server

//i2c
#include "i2cDevice.h"
#include "i2cManager.h"

//i2c devices
#include "AmbientTempSensor.h"
#include "RGBColorSensor.h"
#include "MotorController.h"
#include "IRTempSensor.h"

//controller input
#include "controllerInput.h"

//signal handler
#include <signal.h>

/*
 * Main application intended to run on the client device.
 * -Connects to the server.
 * -Uses incoming controller data to control connected devices.
 * -Sends periodic status reports from each connected device.
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

    //connect to server
    //192.168.7.1
    tcpSocket server("10.10.0.1", 2222);

    //create device manager
    i2cManager i2cM(server);

    //create devices
    AmbientTempSensor ats(i2cM);
    RGBColorSensor rgb(i2cM);
    IRTempSensor ir(i2cM);

    MotorController motor0(i2cM, 0);
    MotorController motor1(i2cM, 1);
    MotorController motor2(i2cM, 2);
    MotorController motor3(i2cM, 3);
    MotorController motor4(i2cM, 4);
    MotorController motor5(i2cM, 5);

    //create controller input
    controllerInput ctrl(server);

    while(running)
    {
        //check for new controller values
        ctrl.updateValues();

        motor0.setLamp(ctrl.data.r1 * 0xFF);
        motor1.setLamp(ctrl.data.r1 * 0xFF);
        motor2.setLamp(ctrl.data.r1 * 0xFF);
        motor3.setLamp(ctrl.data.r1 * 0xFF);
        motor4.setLamp(ctrl.data.r1 * 0xFF);
        motor5.setLamp(ctrl.data.r1 * 0xFF);


        //left
        int leftspeed = ctrl.data.left_y;
        if (leftspeed < 0) leftspeed *= -1;
        leftspeed *= 2;
        if (ctrl.data.left_y > 0) leftspeed *= -1;
        motor0.setSpeed(leftspeed);
        motor2.setSpeed(leftspeed);
        motor4.setSpeed(leftspeed);

        //right
        int rightspeed = ctrl.data.right_y;
        if (rightspeed < 0) rightspeed *= -1;
        rightspeed *= 2;
        if (ctrl.data.right_y < 0) rightspeed *= -1;
        motor1.setSpeed(rightspeed);
        motor3.setSpeed(rightspeed);
        motor5.setSpeed(rightspeed);


        //send device data to server
        i2cM.sendData();

        //sleep for 100 ms
        //usleep(100 * 1000);
    }

    std::cout << "stopping..." << std::endl;
    return 0;

}
