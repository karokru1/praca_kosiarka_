#ifndef BH1750_HPP
#define BH1750_HPP

#include <cstdint>
#include <cstddef>

class BH1750 
{
public:
    bool begin();
    float readLightLevel();

private:
    int i2c_fd = -1;
    const char* i2c_device = "/dev/i2c-1";
    uint8_t address = 0x23; 
    bool writeByte(uint8_t byte);
    bool readBytes(uint8_t* buffer, size_t length);
};

#endif //BH1750_HPP
