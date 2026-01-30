#ifndef TEMPERATURESENSOR_HPP
#define TEMPERATURESENSOR_HPP

class bme280Interface;

class TemperatureSensor
{
public:
    TemperatureSensor(bme280Interface& bme280Ref);
    float read();

private:
    bme280Interface& bme280;
};

#endif //TEMPERATURESENSOR_HPP
