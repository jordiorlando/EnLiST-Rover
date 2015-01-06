#include "i2cManager.h"
#include "i2cDevice.h"

i2cDevice::i2cDevice(i2cManager & manager, int address, int updateRate)
{
    this->updateRate = updateRate;
    deviceAddress = address;
    //TODO: check that the address is valid (or else)

    deviceManager = &manager;

    i2c_fd = deviceManager->initDevice(*this);
}

i2cDevice::~i2cDevice()
{
    deviceManager->removeDevice(*this);
}

int i2cDevice::getAddress()
{
    return deviceAddress;
}

int i2cDevice::smbWrite(uint8_t value)
{
    setAddress();

    return i2c_smbus_write_byte(i2c_fd, value);
}

/*
FOR FUTURE REFERENCE/IMPLEMENTATION
ioctl(file, I2C_RDWR, struct i2c_rdwr_ioctl_data *msgset)
Do combined read/write transaction without stop in between.
Only valid if the adapter has I2C_FUNC_I2C.  The argument is
a pointer to a

struct i2c_rdwr_ioctl_data {
struct i2c_msg *msgs;  // ptr to array of simple messages
int nmsgs;             // number of messages to exchange
}

The msgs[] themselves contain further pointers into data buffers.
The function will write or read data to or from that buffers depending
on whether the I2C_M_RD flag is set in a particular message or not.
The slave address and whether to use ten bit address mode has to be
set in each message, overriding the values set with the above ioctl's.
*/

int i2cDevice::smbRead()
{
    setAddress();

    return i2c_smbus_read_byte(i2c_fd);
}

int i2cDevice::smbWriteByte(uint8_t reg, uint8_t value)
{
    setAddress();

    return i2c_smbus_write_byte_data(i2c_fd, reg, value);
}

int i2cDevice::smbReadByte(uint8_t reg)
{
    setAddress();

    return i2c_smbus_read_byte_data(i2c_fd, reg);
}

int i2cDevice::smbWriteWord(uint8_t reg, uint16_t value)
{
    setAddress();

    return i2c_smbus_write_word_data(i2c_fd, reg, value);
}

/*
 * Read a 16 bit word at the specified register.
 * Returns the word on success, otherwise negative errno.
 */
int i2cDevice::smbReadWord(uint8_t reg)
{
    setAddress();

    return i2c_smbus_read_word_data(i2c_fd, reg);
}

/*
 * Write at most 32 bytes as a block over SMBus at the specified register.
 * Returns 0 on success, otherwise negative errno.
 */
int i2cDevice::smbWriteBlock(uint8_t reg, uint8_t * data, size_t length)
{
    setAddress();

    return i2c_smbus_write_block_data(i2c_fd, reg, length, data);
}

/*
 * Read up to 32 byte block returned by SMBus.
 * Returns number of bytes read on success, otherwise negative errno.
 */
int i2cDevice::smbReadBlock(uint8_t reg, uint8_t * data)
{
    setAddress();

    return i2c_smbus_read_block_data(i2c_fd, reg, data);
}

ssize_t i2cDevice::i2cWrite(uint8_t * data, size_t length)
{
    //set to the current device address with ioctl
    setAddress();

    ssize_t t = write(i2c_fd, data, length);
    if (t != length)
    {
        //write() error
        std::cout << "i2cWrite write" << t << std::endl;
    }

    return t;
}

ssize_t i2cDevice::i2cRead(uint8_t * data, size_t length)
{
    //set to the current device address with ioctl
    setAddress();

    ssize_t t = read(i2c_fd, data, length);
    if (t != length)
    {
        //read() error
        std::cout << "i2cRead read" << std::endl;
    }

    return t;
}

void i2cDevice::setAddress()
{
    if (ioctl(i2c_fd, I2C_SLAVE, deviceAddress) < 0)
    {
        //ioctl() error
        std::cout << "ioctl" << std::endl;
    }
}

/*
 * Returns if the device is actually connected.
 */
bool i2cDevice::isConnected()
{
    return connected;
}

/*
 * Returns if there is new data to send.
 */
bool i2cDevice::dataReady()
{
    time_t t = time(0);   // get time now
    return (difftime(t, lastUpdate) * 1000 >= updateRate);
}

/*
 * Updates internal device data and status.
 */
void i2cDevice::updateData()
{
    lastUpdate = time(0);
    _update();
}

/*
 * This should be implemented by all derived classes to update
 * device data and connectivity status. The following is implemented
 * to allow for the use of a generic i2c object.
 */
void i2cDevice::_update()
{
    connected = (smbRead() >= 0);
}

/*
 * Writes a buffer with the status to send and returns the length
 * of the data written.
 */
int i2cDevice::dataOut(uint8_t * buf)
{
    updateData();
    return _out(buf);
}

/*
 * This should be implemented by all derived classes to output
 * the data to be sent to the server. The function should return
 * the length of the data written to the buffer.
 */
int i2cDevice::_out(uint8_t * buf)
{
    return 0;
}
