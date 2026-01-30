#include "RainSensor.hpp"
#include <mcp3008.hpp>

RainSensor::RainSensor(MCP3008& adc, int channel, int threshold)
    : adc(adc), channel(channel), threshold(threshold) 
{

}

bool RainSensor::isRaining() 
{
    int value = adc.readChannel(channel);
    return value >= 0 && value < threshold;
}
