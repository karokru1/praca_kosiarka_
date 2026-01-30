#include "UserExpectation.hpp"
#include <iostream>
#include <string>

float UserExpectation::setHeight(float grassHeight)
{
    while (true)
    {
        std::cout << "Podaj maksymalna wysokosc trawy [cm] (5 - 15): " << std::endl;
        std::string input;
        if (!std::getline(std::cin, input))
        {
            return m_grassHeight;
        }

        for (char &ch : input)
        {
            if (ch == ',')
            {
                ch = '.';
            }
        }

        try
        {
            grassHeight = std::stof(input);
        }
        catch (const std::exception &)
        {
            std::cout << "Podana wysokosc jest poza zakresem (min 5 cm, max 15 cm)" << std::endl;
            continue;
        }

        if (grassHeight >= m_minGrassHeight && grassHeight <= m_maxGrassHeight)
        {
            m_grassHeight = grassHeight;
            return m_grassHeight;
        }

        std::cout << "Podana wysokosc jest poza zakresem (min 5 cm, max 15 cm)" << std::endl;
    }
}

float UserExpectation::getHeight()
{
    return m_grassHeight;
}

float UserExpectation::getMinGrassHeight()
{
    return m_minGrassHeight;
}

float UserExpectation::getMaxGrassHeight()
{
    return m_maxGrassHeight;
}
