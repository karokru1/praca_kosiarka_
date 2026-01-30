#include "bh1750.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <cstring>

bool BH1750::begin() 
{
    i2c_fd = open(i2c_device, O_RDWR);
    if (i2c_fd < 0) 
    {
        std::cerr << "[BH1750] Nie mozna otworzyc i2c: " << strerror(errno) << std::endl;
        return false;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) 
    {
        std::cerr << "[BH1750] Nie mozna ustawic adresu: " << strerror(errno) << std::endl;
        return false;
    }

    return writeByte(0x01) && writeByte(0x10);
}


float BH1750::readLightLevel() 
{
    uint8_t buffer[2] = {0};
    if (!readBytes(buffer, 2)) 
    {
        return -1.0f;
    }

    uint16_t raw = (buffer[0] << 8) | buffer[1];
    return raw / 1.2f; 
}


bool BH1750::writeByte(uint8_t byte) 
{
    if (write(i2c_fd, &byte, 1) != 1) 
    {
        std::cerr << "[BH1750] Blad zapisu" << std::endl;
        return false;
    }

    return true;
}


bool BH1750::readBytes(uint8_t* buffer, size_t length) 
{
    if (read(i2c_fd, buffer, length) != (ssize_t)length) 
    {
        std::cerr << "[BH1750] Blad odczytu" << std::endl;
        return false;
    }
    
    return true;
}