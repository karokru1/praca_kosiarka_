#include "mcp3008.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <iostream>

MCP3008::MCP3008(const char* spiDevice, uint32_t speed)
    : device(spiDevice), speedHz(speed), spiFd(-1) 
{

}

MCP3008::~MCP3008() 
{
    if (spiFd >= 0)
    {
        close(spiFd);
    }
}

bool MCP3008::begin()
 {
    spiFd = open(device, O_RDWR);
    if (spiFd < 0) 
    {
        std::cerr << "[MCP3008] Błąd otwarcia SPI: " << strerror(errno) << std::endl;
        return false;
    }

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;

    if (ioctl(spiFd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(spiFd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
        ioctl(spiFd, SPI_IOC_WR_MAX_SPEED_HZ, &speedHz) < 0) 
    {
        std::cerr << "[MCP3008] Błąd konfiguracji SPI" << std::endl;
        return false;
    }

    return true;
}

int MCP3008::readChannel(int channel) 
{
    if (channel < 0 || channel > 7) 
    {
        return -1;
    }

    uint8_t tx[] = {
        1,                          
        static_cast<uint8_t>((8 + channel) << 4), 
        0
    };
    uint8_t rx[3] = {0};

    struct spi_ioc_transfer tr = {};
        tr.tx_buf = reinterpret_cast<unsigned long>(tx);
        tr.rx_buf = reinterpret_cast<unsigned long>(rx);
        tr.len = 3;
        tr.delay_usecs = 0;
        tr.speed_hz = speedHz;
        tr.bits_per_word = 8;

    if (ioctl(spiFd, SPI_IOC_MESSAGE(1), &tr) < 0) 
    {
        std::cerr << "[MCP3008] Blad transferu SPI" << std::endl;
        return -1;
    }

    int result = ((rx[1] & 3) << 8) | rx[2];
    return result;
}
