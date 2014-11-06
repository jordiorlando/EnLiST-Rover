#include "i2cDevice.h"

i2cDevice::i2cDevice(i2cManager & manager, int address)
{
    deviceAddress = address;
    //deviceManager = manager;
    
    i2c_fd = manager.initDevice(*this);
    
}

i2cDevice::~i2cDevice()
{
    //deviceManager.removeDevice(*this);
}
  
  /*
  libi2c-dev smbus protocol functions:
  (not implemented)
  __s32 i2c_smbus_write_quick(int file, __u8 value);
  __s32 i2c_smbus_process_call(int file, __u8 command, __u16 value);
  (implemented)
  __s32 i2c_smbus_read_byte(int file);
  __s32 i2c_smbus_write_byte(int file, __u8 value);
  __s32 i2c_smbus_read_byte_data(int file, __u8 command);
  __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value);
  __s32 i2c_smbus_read_word_data(int file, __u8 command);
  __s32 i2c_smbus_write_word_data(int file, __u8 command, __u16 value);
  __s32 i2c_smbus_read_block_data(int file, __u8 command, __u8 *values);
  __s32 i2c_smbus_write_block_data(int file, __u8 command, __u8 length, 
                                   __u8 *values);
  */

int i2cDevice::smbWrite(uint8_t value)
{
    setAddress();

    return i2c_smbus_write_byte(i2c_fd, value);
}

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

int i2cDevice::smbReadWord(uint8_t reg)
{
    setAddress();
    
    return i2c_smbus_read_word_data(i2c_fd, reg);
}

/* 
 * Write at most 32 bytes as a block over SMBus.
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
int i2cDevice::smbReadBlock(uint8_t reg, uint8_t * data);
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