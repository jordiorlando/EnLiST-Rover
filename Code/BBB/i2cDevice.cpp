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

//read = 1, write = 0
//command/reg
//size isn't used unless 
/*
extern inline s32 i2c_smbus_access(char read_write, uint8_t command, 
                                   int size, union smbus_data *data)
{
  struct i2c_smbus_data args;
  int res;

  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;
  return ioctl(i2c_fd,I2C_SMBUS,&args);
}

extern inline s32 i2c_smbus_read_byte_data(int file, u8 command)
{
  union smbus_data data;
  if (i2c_smbus_access(SMBUS_READ,command,SMBUS_BYTE_DATA,&data))
    return -1;
  else
    return 0x0FF & data.byte;
}
*/

std::vector<unsigned char> i2cDevice::i2cRead(unsigned char reg, size_t length)
{
    std::vector<unsigned char> ret(length);
	
    //set to the current device address with ioctl
	setAddress();
    
	ssize_t s = write(i2c_fd, &reg, 1);
	if (s != 1) {
		//write() error
        std::cout << "write1" << std::endl;
	}
    
	ssize_t t = read(i2c_fd, ret.data(), length);
	if (t != length)
	{
		//read() error
        std::cout << "read1" << std::endl;
	}
		
	return ret;

}

void i2cDevice::i2cWrite(unsigned char reg, std::vector<unsigned char> & data)
{
    //set to the current device address with ioctl
	setAddress();
    
    //not ideal, insert at front, maybe replace?
    data.insert(data.begin(), reg);
    

	size_t t = write(i2c_fd, data.data(), data.size());
    if (t != data.size())
    {
		//write() error
        std::cout << "read2 " << t << std::endl;
	}

}

void i2cDevice::setAddress()
{
    if (ioctl(i2c_fd, I2C_SLAVE, deviceAddress) < 0) {
		//ioctl() error
        std::cout << "ioctl" << std::endl;
	}
}