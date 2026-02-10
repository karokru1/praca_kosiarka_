#include <gtest/gtest.h>
#include "Decision.hpp"
#include "MockSensorProvider.hpp"

TEST(DecisionTest, allowsCuttingWhenConditionsGood)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 500.0f, 400.0f, 12.0f, false};

    EXPECT_TRUE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenRaining)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 500.0f, 400.0f, 8.0f, true};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenGrassTooShort)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 500.0f, 400.0f, 3.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenGrassBetweenThresholds)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 500.0f, 400.0f, 6.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenSoilMoistureTooHigh)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 500.0f, 900.0f, 12.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenHumidityTooHigh)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 95.0f, 1013.0f, 500.0f, 400.0f, 12.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenLightTooLow)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 50.0f, 400.0f, 12.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenTemperatureTooLow)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {5.0f, 50.0f, 1013.0f, 500.0f, 400.0f, 12.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenTemperatureTooHigh)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {40.0f, 50.0f, 1013.0f, 500.0f, 400.0f, 12.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, AllowsCuttingWhenGrassAtMaxThreshold)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 500.0f, 400.0f, 10.0f, false};

    EXPECT_TRUE(decision.shouldCutGrass(sensor.readAll()));
}

TEST(DecisionTest, blocksWhenGrassHeightUnavailable)
{
    Decision decision;
    MockSensorProvider sensor;
    sensor.data = {20.0f, 50.0f, 1013.0f, 500.0f, 400.0f, -1.0f, false};

    EXPECT_FALSE(decision.shouldCutGrass(sensor.readAll()));
}
