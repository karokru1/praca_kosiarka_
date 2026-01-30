#ifndef HUMIDITYSENSOR_HPP
#define HUMIDITYSENSOR_HPP

class bme280Interface;

class HumiditySensor 
{
public:
    HumiditySensor(bme280Interface& bme280Ref);
    float read();

private:
    bme280Interface& bme280;
};

#endif //HUMIDITYSENSOR_HPP