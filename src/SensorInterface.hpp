#ifndef SENSORINTERFACE_HPP
#define SENSORINTERFACE_HPP

#include "Decision.hpp"

class SensorInterface 
{
public:
    virtual ~SensorInterface() = default;
    virtual SensorReadings readAll() = 0;
};

#endif //SENSORINTERFACE_HPP
