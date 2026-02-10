#include "bme280.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <cstring>

BME280::BME280(const char* i2cDevice, uint8_t address)
    : device(i2cDevice),
      address(address),
      i2cFd(-1),
      initialized(false),
      tFine(0),
      dig_T1(0),
      dig_T2(0),
      dig_T3(0),
      dig_P1(0),
      dig_P2(0),
      dig_P3(0),
      dig_P4(0),
      dig_P5(0),
      dig_P6(0),
      dig_P7(0),
      dig_P8(0),
      dig_P9(0),
      dig_H1(0),
      dig_H2(0),
      dig_H3(0),
      dig_H4(0),
      dig_H5(0),
      dig_H6(0)
{
}

BME280::~BME280()
{
    if (i2cFd >= 0)
    {
        close(i2cFd);
    }
}

bool BME280::begin()
{
    i2cFd = open(device, O_RDWR);
    if (i2cFd < 0)
    {
        std::cerr << "BME280 Nie mozna otworzyc " << device << ": " << strerror(errno) << std::endl;
        return false;
    }

    if (ioctl(i2cFd, I2C_SLAVE, address) < 0)
    {
        std::cerr << "BME280 Nie mozna ustawic adresu: " << strerror(errno) << std::endl;
        return false;
    }

    uint8_t chipId = 0;
    if (!readRegisters(0xD0, &chipId, 1) || chipId != 0x60)
    {
        std::cerr << "BME280 Nieprawidlowy ID ukladu " << std::hex << static_cast<int>(chipId) << std::dec << ")" << std::endl;
        return false;
    }

    if (!writeRegister(0xE0, 0xB6))
    {
        return false;
    }
    usleep(2000);

    if (!readCalibrationData())
    {
        return false;
    }

    if (!writeRegister(0xF2, 0x01))
    {
        return false;
    }

    if (!writeRegister(0xF4, 0x27))
    {
        return false;
    }

    if (!writeRegister(0xF5, 0xA0))
    {
        return false;
    }

    initialized = true;
    return true;
}

bool BME280::readCalibrationData()
{
    uint8_t calib[26] = {0};
    if (!readRegisters(0x88, calib, sizeof(calib)))
    {
        return false;
    }

    dig_T1 = (calib[1] << 8) | calib[0];
    dig_T2 = static_cast<int16_t>((calib[3] << 8) | calib[2]);
    dig_T3 = static_cast<int16_t>((calib[5] << 8) | calib[4]);
    dig_P1 = (calib[7] << 8) | calib[6];
    dig_P2 = static_cast<int16_t>((calib[9] << 8) | calib[8]);
    dig_P3 = static_cast<int16_t>((calib[11] << 8) | calib[10]);
    dig_P4 = static_cast<int16_t>((calib[13] << 8) | calib[12]);
    dig_P5 = static_cast<int16_t>((calib[15] << 8) | calib[14]);
    dig_P6 = static_cast<int16_t>((calib[17] << 8) | calib[16]);
    dig_P7 = static_cast<int16_t>((calib[19] << 8) | calib[18]);
    dig_P8 = static_cast<int16_t>((calib[21] << 8) | calib[20]);
    dig_P9 = static_cast<int16_t>((calib[23] << 8) | calib[22]);
    dig_H1 = calib[25];

    uint8_t calibH[7] = {0};
    if (!readRegisters(0xE1, calibH, sizeof(calibH)))
    {
        return false;
    }

    dig_H2 = static_cast<int16_t>((calibH[1] << 8) | calibH[0]);
    dig_H3 = calibH[2];
    dig_H4 = static_cast<int16_t>((calibH[3] << 4) | (calibH[4] & 0x0F));
    dig_H5 = static_cast<int16_t>((calibH[5] << 4) | (calibH[4] >> 4));
    dig_H6 = static_cast<int8_t>(calibH[6]);

    return true;
}

bool BME280::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t buffer[2] = {reg, value};
    if (write(i2cFd, buffer, 2) != 2)
    {
        std::cerr << "BME280 Blad zapisu rejestru 0x" << std::hex << static_cast<int>(reg) << std::dec << std::endl;
        return false;
    }
    return true;
}

bool BME280::readRegisters(uint8_t reg, uint8_t* buffer, size_t length)
{
    if (write(i2cFd, &reg, 1) != 1)
    {
        std::cerr << "BME280 Blad ustawienia rejestru 0x" << std::hex << static_cast<int>(reg) << std::dec << std::endl;
        return false;
    }

    if (read(i2cFd, buffer, length) != static_cast<ssize_t>(length))
    {
        std::cerr << "BME280 Blad odczytu z rejestru 0x" << std::hex << static_cast<int>(reg) << std::dec << std::endl;
        return false;
    }

    return true;
}

int32_t BME280::readRawTemperature()
{
    uint8_t data[3] = {0};
    if (!readRegisters(0xFA, data, sizeof(data)))
    {
        return -1;
    }

    return (static_cast<int32_t>(data[0]) << 12) | (static_cast<int32_t>(data[1]) << 4) | (static_cast<int32_t>(data[2]) >> 4);
}

int32_t BME280::readRawPressure()
{
    uint8_t data[3] = {0};
    if (!readRegisters(0xF7, data, sizeof(data)))
    {
        return -1;
    }

    return (static_cast<int32_t>(data[0]) << 12) | (static_cast<int32_t>(data[1]) << 4) | (static_cast<int32_t>(data[2]) >> 4);
}

int32_t BME280::readRawHumidity()
{
    uint8_t data[2] = {0};
    if (!readRegisters(0xFD, data, sizeof(data)))
    {
        return -1;
    }

    return (static_cast<int32_t>(data[0]) << 8) | static_cast<int32_t>(data[1]);
}

float BME280::readTemperature()
{
    if (!initialized)
    {
        std::cerr << "BME280 Nie zostalo zainicjalizowane" << std::endl;
        return -100.0f;
    }

    int32_t adc_T = readRawTemperature();
    if (adc_T < 0)
    {
        return -100.0f;
    }

    int32_t var1 = ((((adc_T >> 3) - (static_cast<int32_t>(dig_T1) << 1))) * static_cast<int32_t>(dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - static_cast<int32_t>(dig_T1)) * ((adc_T >> 4) - static_cast<int32_t>(dig_T1))) >> 12) * static_cast<int32_t>(dig_T3)) >> 14;

    tFine = var1 + var2;
    float temp = (tFine * 5 + 128) >> 8;
    return temp / 100.0f;
}

float BME280::readPressure()
{
    if (!initialized)
    {
        std::cerr << "BME280 Nie zostalo zainicjalizowane" << std::endl;
        return -1.0f;
    }

    if (tFine == 0)
    {
        readTemperature();
    }

    int32_t adc_P = readRawPressure();
    if (adc_P < 0)
    {
        return -1.0f;
    }

    int64_t var1 = static_cast<int64_t>(tFine) - 128000;
    int64_t var2 = var1 * var1 * static_cast<int64_t>(dig_P6);
    var2 = var2 + ((var1 * static_cast<int64_t>(dig_P5)) << 17);
    var2 = var2 + (static_cast<int64_t>(dig_P4) << 35);
    var1 = ((var1 * var1 * static_cast<int64_t>(dig_P3)) >> 8) + ((var1 * static_cast<int64_t>(dig_P2)) << 12);
    var1 = (((static_cast<int64_t>(1) << 47) + var1) * static_cast<int64_t>(dig_P1)) >> 33;

    if (var1 == 0)
    {
        return -1.0f;
    }

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (static_cast<int64_t>(dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (static_cast<int64_t>(dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (static_cast<int64_t>(dig_P7) << 4);

    return static_cast<float>(p) / 25600.0f; // hPa
}

float BME280::readHumidity()
{
    if (!initialized)
    {
        std::cerr << "BME280 Nie zostalo zainicjalizowane" << std::endl;
        return -1.0f;
    }

    if (tFine == 0)
    {
        readTemperature();
    }

    int32_t adc_H = readRawHumidity();
    if (adc_H < 0)
    {
        return -1.0f;
    }

    int32_t v_x1_u32r = tFine - 76800;
    v_x1_u32r = (((((adc_H << 14) - (static_cast<int32_t>(dig_H4) << 20) - (static_cast<int32_t>(dig_H5) * v_x1_u32r)) + 16384) >> 15) *
                 (((((((v_x1_u32r * static_cast<int32_t>(dig_H6)) >> 10) * (((v_x1_u32r * static_cast<int32_t>(dig_H3)) >> 11) + 32768)) >> 10) + 2097152) *
                    static_cast<int32_t>(dig_H2) + 8192) >> 14));
    v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * static_cast<int32_t>(dig_H1)) >> 4);
    v_x1_u32r = v_x1_u32r < 0 ? 0 : v_x1_u32r;
    v_x1_u32r = v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r;

    float hum = (v_x1_u32r >> 12) / 1024.0f;
    return hum;
}
