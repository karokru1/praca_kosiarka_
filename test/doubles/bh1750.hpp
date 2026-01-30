#ifndef TEST_DOUBLE_BH1750_HPP
#define TEST_DOUBLE_BH1750_HPP

class BH1750
{
public:
    static void setNext(float value) 
    { 
        nextValue = value; 
    }

    bool begin() 
    { 
        return true; 
    }

    float readLightLevel() 
    { 
        return nextValue; 
    }

private:
    inline static float nextValue = 0.0f;
};

#endif // TEST_DOUBLE_BH1750_HPP
