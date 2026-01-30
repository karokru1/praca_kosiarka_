#include "bme280Interface.hpp"

bool bme280Interface::begin() 
{
    return bme280.begin();
}

float bme280Interface::readTemperature() 
{
    return bme280.readTemperature();
}

float bme280Interface::readHumidity() 
{
    return bme280.readHumidity();
}

float bme280Interface::readPressure() 
{
    return bme280.readPressure();
}
