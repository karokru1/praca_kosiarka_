#ifndef BME280INTERFACE_HPP
#define BME280INTERFACE_HPP

#include "bme280.hpp"

class bme280Interface
{
public:
    bool begin();
    float readTemperature();
    float readHumidity();
    float readPressure();

private:
    BME280 bme280;
};

#endif //BME280INTERFACE_HPP
