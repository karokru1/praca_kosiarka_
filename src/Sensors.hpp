#ifndef SENSORS_HPP
#define SENSORS_HPP

#include "LightSensor.hpp"
#include "TemperatureSensor.hpp"
#include "HumiditySensor.hpp"
#include "RainSensor.hpp"
#include "SoilMoistureSensor.hpp"
#include "PressureSensor.hpp"
#include "bme280Interface.hpp"
#include "SensorInterface.hpp"
#include "mcp3008.hpp"

class Sensors : public SensorInterface
{
public:
    Sensors(int soilChannel = 0, int rainChannel = 1, int rainThreshold = 400);
    bool init();
    SensorReadings readAll() override;
    float getLightLevel();
    float getTemperature();
    float getHumidity();
    float getPressure();
    int getSoilMoisture();
    bool isRaining();

private:
    bool checkInitialized(const char* caller) const;
    bme280Interface bme280;
    MCP3008 adc;
    int soilChannel;
    int rainChannel;
    int rainThreshold;
    TemperatureSensor temp;
    HumiditySensor humid;
    PressureSensor press;
    SoilMoistureSensor soil;
    RainSensor rain;
    LightSensor light;
    bool initialized;
};

#endif //SENSORS_HPP
