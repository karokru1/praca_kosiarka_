#ifndef USEREXPECTATION_HPP
#define USEREXPECTATION_HPP

class UserExpectation
{
public:
    float setHeight(float grassHeight);
    float getHeight();
    float getMinGrassHeight();
    float getMaxGrassHeight();

private:
   float m_grassHeight = 0.f;
   float m_minGrassHeight = 5.f;
   float m_maxGrassHeight = 15.f;
};

#endif //USEREXPECTATION_HPP