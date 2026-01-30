#ifndef RAILSENSOR_HPP
#define RAILSENSOR_HPP

class MCP3008;

class RainSensor 
{
public:
    RainSensor(MCP3008& adc, int channel, int threshold = 700);
    bool isRaining();

private:
    MCP3008& adc;
    int channel;
    int threshold;
};

#endif //RAILSENSOR_HPP