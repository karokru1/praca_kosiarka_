#ifndef DECISION_HPP
#define DECISION_HPP

struct SensorReadings 
{
    float temperature;
    float humidity;
    float pressure;
    float light;
    float soilMoisture;
    float grassHeight;
    bool rainDetected;
};

struct DecisionThresholds
{
    float minTemperature = 10.0f;
    float maxTemperature = 35.0f;
    float minLight = 100.0f;
    float maxSoilMoisture = 800.0f; 
    float maxHumidity = 90.0f; 
    float minGrassHeight = 5.0f;
    float maxGrassHeight = 10.0f;
};

class Decision 
{
public:
    explicit Decision(const DecisionThresholds& thresholds = DecisionThresholds{});
    bool shouldCutGrass(const SensorReadings& data) const;

private:
    DecisionThresholds thresholds;
};

#endif // DECISION_HPP
