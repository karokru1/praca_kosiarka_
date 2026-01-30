#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include "Decision.hpp"

class Logger 
{
public:
    explicit Logger(const std::string& dbPath = "log.db");
    ~Logger();

    bool init();
    bool logReading(const SensorReadings& reading);

private:
    bool execute(const std::string& sql);
    std::string dbPath;
    void* db;
};

#endif //LOGGER_HPP
