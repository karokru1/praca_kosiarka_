#ifndef BME280_HPP
#define BME280_HPP

#include <cstdint>
#include <cstddef>

class BME280
{
public:
    explicit BME280(const char* i2cDevice = "/dev/i2c-1", uint8_t address = 0x76);
    ~BME280();
    bool begin();
    float readTemperature();
    float readHumidity();
    float readPressure();

private:
    bool readCalibrationData();
    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegisters(uint8_t reg, uint8_t* buffer, size_t length);
    int32_t readRawTemperature();
    int32_t readRawPressure();
    int32_t readRawHumidity();

    const char* device;
    uint8_t address;
    int i2cFd;
    bool initialized;
    int32_t tFine;

    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
};

#endif // BME280_HPP
