#ifndef PRESSURESENSOR_HPP
#define PRESSURESENSOR_HPP

class bme280Interface;

class PressureSensor 
{
public:
    PressureSensor(bme280Interface& bme280Ref);
    float read();

private:
    bme280Interface& bme280;
};

#endif //PRESSURESENSOR_HPP