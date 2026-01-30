#include <iostream>
#include "Sensors.hpp"

Sensors::Sensors(int soilChannel, int rainChannel, int rainThreshold)
    : bme280(),
      adc(),
      soilChannel(soilChannel),
      rainChannel(rainChannel),
      rainThreshold(rainThreshold),
      temp(bme280),
      humid(bme280),
      press(bme280),
      soil(adc, soilChannel),
      rain(adc, rainChannel, rainThreshold),
      light(),
      initialized(false)
{
}

bool Sensors::checkInitialized(const char* caller) const
{
    if (initialized) 
    {
        return true;
    }

    std::cerr << "[Sensors] " << caller << " wywolane przed init()" << std::endl;
    return false;
}

bool Sensors::init()
{
    initialized = false;

    if (!bme280.begin()) 
    {
        std::cerr << "[Sensors] Blad inicjalizacji BME280" << std::endl;
        return false;
    }

    if (!adc.begin()) 
    {
        std::cerr << "[Sensors] Blad inicjalizacji MCP3008" << std::endl;
        return false;
    }

    if (!light.init()) 
    {
        std::cerr << "[Sensors] Blad inicjalizacji BH1750" << std::endl;
        return false;
    }

    initialized = true;
    return true;
}

float Sensors::getLightLevel()
{
    if (!checkInitialized("getLightLevel")) 
    {
        return 0.0f;
    }
    return light.read();
}

float Sensors::getTemperature()
{
    if (!checkInitialized("getTemperature")) 
    {
        return 0.0f;
    }
    return temp.read();
}

float Sensors::getHumidity()
{
    if (!checkInitialized("getHumidity")) 
    {
        return 0.0f;
    }
    return humid.read();
}

float Sensors::getPressure()
{
    if (!checkInitialized("getPressure")) 
    {
        return 0.0f;
    }
    return press.read();
}

int Sensors::getSoilMoisture()
{
    if (!checkInitialized("getSoilMoisture")) 
    {
        return 0;
    }
    return soil.read();
}

bool Sensors::isRaining()
{
    if (!checkInitialized("isRaining")) 
    {
        return false;
    }
    return rain.isRaining();
}

SensorReadings Sensors::readAll()
{
    return {getTemperature(), getHumidity(), getPressure(), getLightLevel(), static_cast<float>(getSoilMoisture()), -1.0f, isRaining()};
}
