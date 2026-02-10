#ifndef TEST_DOUBLE_MCP3008_HPP
#define TEST_DOUBLE_MCP3008_HPP

#include <cstdint>
#include <array>

class MCP3008
{
public:
    explicit MCP3008(const char* = "/dev/spidev0.0", uint32_t = 1000000) {}
    ~MCP3008() = default;

    bool begin() { return true; }

    static void setChannelValue(int channel, int value)
    {
        if (channel >= 0 && channel < 8) {
            channelValues[channel] = value;
        }
    }

    int readChannel(int channel)
    {
        if (channel < 0 || channel >= 8) {
            return -1;
        }
        return channelValues[channel];
    }

private:
    inline static std::array<int, 8> channelValues{0,0,0,0,0,0,0,0};
};

#endif // TEST_DOUBLE_MCP3008_HPP
