#include "LightSensor.hpp"
#include <bh1750.hpp>

static BH1750 bh1750;

bool LightSensor::init()
{
    return bh1750.begin();
}

float LightSensor::read()
{
    return bh1750.readLightLevel();
}
