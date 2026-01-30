#include "HumiditySensor.hpp"
#include "bme280Interface.hpp"

HumiditySensor::HumiditySensor(bme280Interface& bme280Ref)
    : bme280(bme280Ref) 
{

}

float HumiditySensor::read() 
{
    return bme280.readHumidity();
}
