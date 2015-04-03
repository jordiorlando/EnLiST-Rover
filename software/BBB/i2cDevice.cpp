#include "i2cDevice.h"

i2cDevice::i2cDevice(int address, deviceManager & manager, int updateRate) :
device(manager, address, updateRate)
{
    i2c_address = address;
    //TODO: check that the address is valid (or else)

    i2c_fd = manager.geti2cBus();
}

int i2cDevice::getAddress()
{
    return i2c_address;
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
    if (ioctl(i2c_fd, I2C_SLAVE, i2c_address) < 0)
    {
        //ioctl() error
        std::cout << "ioctl" << std::endl;
    }
}


void i2cDevice::_update()
{
    connected = (smbRead() >= 0);
}

int i2cDevice::_out(uint8_t * buf)
{
    return 0;
}
