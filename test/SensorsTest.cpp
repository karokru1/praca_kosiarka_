#include <gtest/gtest.h>
#include "MockSensorProvider.hpp"
#include "LightSensor.hpp"
#include "SoilMoistureSensor.hpp"
#include "RainSensor.hpp"
#include "mcp3008.hpp"
#include "bh1750.hpp"

TEST(SensorsTest, mockProviderReturnsData)
{
    MockSensorProvider sensor;
    sensor.data.temperature = 22.0f;
    sensor.data.grassHeight = 7.0f;

    EXPECT_FLOAT_EQ(sensor.readAll().temperature, 22.0f);
    EXPECT_FLOAT_EQ(sensor.readAll().grassHeight, 7.0f);
}

TEST(LightSensorTest, readsStubbedValue)
{
    BH1750::setNext(123.4f);
    LightSensor sensor;

    EXPECT_TRUE(sensor.init());
    EXPECT_FLOAT_EQ(sensor.read(), 123.4f);
}

TEST(SoilMoistureSensorTest, readsFromStubAdc)
{
    MCP3008::setChannelValue(0, 512);
    MCP3008 adc;
    SoilMoistureSensor soil(adc, 0);

    EXPECT_EQ(soil.read(), 512);
}

TEST(RainSensorTest, detectsRainWhenBelowThreshold)
{
    MCP3008::setChannelValue(1, 200);
    MCP3008 adc;
    RainSensor rain(adc, 1, 400);

    EXPECT_TRUE(rain.isRaining());

    MCP3008::setChannelValue(1, 800);
    
    EXPECT_FALSE(rain.isRaining());
}
