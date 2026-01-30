#ifndef SOILMOISTURESENSOR_HPP
#define SOILMOISTURESENSOR_HPP

class MCP3008;

class SoilMoistureSensor 
{
public:
    SoilMoistureSensor(MCP3008& adc, int channel);
    int read();

private:
    MCP3008& adc;
    int channel;
};

#endif //SOILMOISTURESENSOR_HPP