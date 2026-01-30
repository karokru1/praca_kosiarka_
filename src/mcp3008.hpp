#ifndef MCP3008_HPP
#define MCP3008_HPP

#include <cstdint>

class MCP3008 
{
public:
    explicit MCP3008(const char* spiDevice = "/dev/spidev0.0", uint32_t speed = 1000000);
    ~MCP3008();
    bool begin();
    int readChannel(int channel);
private:
    const char* device;
    uint32_t speedHz;
    int spiFd;
};

#endif //MCP3008_HPP