#include "Decision.hpp"

Decision::Decision(const DecisionThresholds& thresholds)
    : thresholds(thresholds)
{
}

bool Decision::shouldCutGrass(const SensorReadings& data) const
{
    if (data.rainDetected) 
    {
        return false;
    }

    if (data.soilMoisture > thresholds.maxSoilMoisture) 
    {
        return false;
    }

    if (data.humidity > thresholds.maxHumidity) 
    {
        return false;
    }

    if (data.light < thresholds.minLight) 
    {
        return false;
    }

    if (data.temperature < thresholds.minTemperature || data.temperature > thresholds.maxTemperature) 
    {
        return false;
    }

    if (data.grassHeight >= 0.0f) 
    {
        if (data.grassHeight < thresholds.minGrassHeight) 
        {
            return false;
        }
        if (data.grassHeight >= thresholds.maxGrassHeight) 
        {
            return true;
        }

        return false;
    }

    return false;
}
