#include <iostream>
#include <locale>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include "UserExpectation.hpp"
#include "Sensors.hpp"
#include "Decision.hpp"
#include "Logger.hpp"
#include "Camera.hpp"

void info()
{
    std::cout << "Projekt kosiarka" << std::endl;
}

int main()
{
    setlocale(LC_ALL, "pl_PL.UTF-8");

    info();

    UserExpectation userData{};
    float firstValue = 0.0f;
    userData.setHeight(firstValue);
    float userHeightValue = userData.getHeight();

    std::filesystem::create_directories("data");

    Logger logger("data/log.db");
    if (!logger.init())
    {
        std::cerr << "Nie udalo sie otworzyc bazy danych" << std::endl;
    }

    float temperature = 0.0f;
    float humidity    = 0.0f;
    float pressure    = 0.0f;
    float light       = 0.0f;
    int   soilMoisture = 0;
    bool rainDetected = false;
    float grassHeight = -1.0f;

    Sensors sensors;
    Camera camera(0, 25.0f);

    if (!sensors.init())
    {
        std::cerr << "Blad podczas inicjalizacji" << std::endl;
        return 1;
    }

    temperature = sensors.getTemperature();
    humidity    = sensors.getHumidity();
    pressure    = sensors.getPressure();
    light       = sensors.getLightLevel();
    soilMoisture = sensors.getSoilMoisture();
    rainDetected = sensors.isRaining();

    if (camera.init()) {
        float measured = camera.estimateGrassHeightCm();
        if (measured >= 0.0f) {
            grassHeight = measured;
        } else {
            std::cerr << "Nie udalo sie zmierzyc wysokosci trawy" << std::endl;
        }
    } else {
        std::cerr << "Pomijam pomiar trawy" << std::endl;
    }

    std::cout << std::fixed;
    std::cout << "Temperatura: " << std::setprecision(2) << temperature << " C" << std::endl;
    std::cout << "Wilgotnosc: " << std::setprecision(2) << humidity    << " %"  << std::endl;
    std::cout << "Cisnienie: " << std::setprecision(1) << pressure    << " hPa" << std::endl;
    std::cout << "Swiatlo: " << std::setprecision(2) << light       << " lux" << std::endl;
    std::cout << "Wilg. gleby: " << soilMoisture  << "%" << std::endl;
    std::cout << "Deszcz: " << (rainDetected ? "tak" : "nie") << std::endl;
    if (grassHeight >= 0.0f) {
        std::cout << "Wysokosc trawy (kamera): " << std::setprecision(1) << grassHeight << " cm" << std::endl;
    }

    SensorReadings readings
    {
        .temperature   = temperature,
        .humidity      = humidity,
        .pressure      = pressure,
        .light         = light,
        .soilMoisture  = static_cast<float>(soilMoisture),
        .grassHeight   = grassHeight,
        .rainDetected  = rainDetected
    };

    DecisionThresholds thresholds;
    thresholds.minGrassHeight = userData.getMinGrassHeight();
    thresholds.maxGrassHeight = userHeightValue > 0.0f ? userHeightValue : thresholds.maxGrassHeight;

    Decision decision(thresholds);
    if (decision.shouldCutGrass(readings))
    {
        std::cout << "Warunki dobre mozna kosic trawe" << std::endl;
    }
    else
    {
        std::cout << "Warunki nieodpowiednie nie kosic trawy" << std::endl;
    }

    logger.logReading(readings);
    std::cout << "Dane zapisane do bazy" << std::endl;

    return 0;
}
