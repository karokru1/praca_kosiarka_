#include <iostream>
#include <locale>
#include <filesystem>
#include <iomanip>
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
        std::cerr << "[Logger] Nie udalo sie otworzyc bazy danych!" << std::endl;
    }

    Sensors sensors;
    Camera camera(0, 25.0f);

    if (!sensors.init())
    {
        std::cerr << "Blad podczas inicjalizacji sensorow!" << std::endl;
        return 1;
    }

    float temperature = sensors.getTemperature();
    float humidity    = sensors.getHumidity();
    float pressure    = sensors.getPressure();
    float light       = sensors.getLightLevel();
    int   soilMoisture = sensors.getSoilMoisture();
    bool rainDetected = sensors.isRaining();
    float grassHeight = -1.0f;

    if (camera.init()) {
        float measured = camera.estimateGrassHeightCm();
        if (measured >= 0.0f) {
            grassHeight = measured;
        } else {
            std::cerr << "[Camera] Nie udalo sie zmierzyc wysokosci trawy" << std::endl;
        }
    } else {
        std::cerr << "[Camera] Pomijam pomiar kamery (brak inicjalizacji)" << std::endl;
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << " Temperatura: " << temperature << " C" << std::endl;
    std::cout << " Wilgotnosc:  " << humidity    << " %"  << std::endl;
    std::cout << " Cisnienie:   " << pressure    << " hPa" << std::endl;
    std::cout << " Swiatlo:     " << light       << " lux" << std::endl;
    std::cout << " Wilg. gleby: " << soilMoisture  << " /1023" << std::endl;
    std::cout << " Deszcz:      " << (rainDetected ? "TAK" : "NIE") << std::endl;
    if (grassHeight >= 0.0f) {
        std::cout << " Wysokosc trawy (kamera): " << grassHeight << " cm" << std::endl;
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
        std::cout << " Warunki dobre mozna kosic trawe." << std::endl;
    }
    else
    {
        std::cout << " Warunki nie są odpowiednie nie kosic trawy." << std::endl;
    }

    logger.logReading(readings);
    std::cout << "[Logger] Dane zapisane do bazy!" << std::endl;

    return 0;
}
