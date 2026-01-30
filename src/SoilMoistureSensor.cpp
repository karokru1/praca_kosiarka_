#include "SoilMoistureSensor.hpp"
#include <mcp3008.hpp>

SoilMoistureSensor::SoilMoistureSensor(MCP3008& adc, int channel)
: adc(adc), channel(channel) 
{

}

int SoilMoistureSensor::read() 
{
    return adc.readChannel(channel);
}
