#ifndef DEVICE_H_
#define DEVICE_H_

//forward declaration of deviceManager
class deviceManager;

/*
 * Abstract base class for a device.
 */
class device
{

    public:
        device(deviceManager & manager, int id, int updateRate);
        virtual ~device();

        //returns whether the device is connected
        bool isConnected();

        //returns the 15 bit device identifier
        uint16_t getDeviceID();

        //returns whether data is ready based on updateRate
        bool needsUpdate();

        //updates internal data and places it in buf (regardless of needsUpdate)
        int update(uint8_t * buf);

    protected:
        bool connected;
        int updaterate;
        uint16_t deviceid;

        //implemented for each device:
        //  called when a device's i/o should be updated/flushed
        virtual void _update();
        //  called when a device's status/data should be placed in buf
        virtual int _out(uint8_t * buf);

    private:
        deviceManager * _manager;

        time_t lastUpdate;
        void performUpdate();

};


#endif //DEVICE_H_
