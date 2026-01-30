#ifndef MOCKSENSORPROVIDER_HPP
#define MOCKSENSORPROVIDER_HPP

#include "SensorInterface.hpp"

class MockSensorProvider : public SensorInterface 
{
public:
    SensorReadings data;
    SensorReadings readAll() override
    {
        return data;
    }
};

#endif //MOCKSENSORPROVIDER_HPP
