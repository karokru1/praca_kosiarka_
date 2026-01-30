#include "TemperatureSensor.hpp"
#include "bme280Interface.hpp"

TemperatureSensor::TemperatureSensor(bme280Interface& bme280Ref)
    : bme280(bme280Ref) 
{
}

float TemperatureSensor::read() 
{
    return bme280.readTemperature();
}