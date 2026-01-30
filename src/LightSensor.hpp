#ifndef LIGHTSENSOR_HPP
#define LIGHTSENSOR_HPP

class LightSensor 
{
public:
    bool init();
    float read();

private:
    float m_lightValue = 0.0f;
};

#endif //LIGHTSENSOR_HPP