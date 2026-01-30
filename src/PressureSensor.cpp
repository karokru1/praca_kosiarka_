#include "PressureSensor.hpp"
#include "bme280Interface.hpp"

PressureSensor::PressureSensor(bme280Interface& bme280Ref)
    : bme280(bme280Ref) 
{
}

float PressureSensor::read() 
{
    return bme280.readPressure();
}
