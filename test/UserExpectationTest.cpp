#include <gtest/gtest.h>
#include "UserExpectation.hpp"
#include <sstream>

TEST(UserExpectationTest, checkHeight)
{
    UserExpectation userData;
    {
        std::istringstream input("10.0\n");
        std::streambuf* origCin = std::cin.rdbuf();
        std::cin.rdbuf(input.rdbuf());
        float result = userData.setHeight(0);
        std::cin.rdbuf(origCin);       
        
        EXPECT_FLOAT_EQ(result, 10.0f);
        EXPECT_FLOAT_EQ(userData.getHeight(), 10.0f);
    }
}

TEST(UserExpectationTest, acceptsCommaAsDecimalSeparator)
{
    UserExpectation userData;
    std::istringstream input("7,5\n");
    std::streambuf* origCin = std::cin.rdbuf();
    std::cin.rdbuf(input.rdbuf());

    float result = userData.setHeight(0);

    std::cin.rdbuf(origCin);
    EXPECT_FLOAT_EQ(result, 7.5f);
    EXPECT_FLOAT_EQ(userData.getHeight(), 7.5f);
}

TEST(UserExpectationTest, retriesOnNonNumericInput)
{
    UserExpectation userData;
    std::istringstream input("abc\n9\n");
    std::streambuf* origCin = std::cin.rdbuf();
    std::cin.rdbuf(input.rdbuf());

    float result = userData.setHeight(0);

    std::cin.rdbuf(origCin);
    EXPECT_FLOAT_EQ(result, 9.0f);
    EXPECT_FLOAT_EQ(userData.getHeight(), 9.0f);
}

TEST(UserExpectationTest, retriesOnOutOfRangeInput)
{
    UserExpectation userData;
    std::istringstream input("20\n8\n");
    std::streambuf* origCin = std::cin.rdbuf();
    std::cin.rdbuf(input.rdbuf());

    float result = userData.setHeight(0);

    std::cin.rdbuf(origCin);
    EXPECT_FLOAT_EQ(result, 8.0f);
    EXPECT_FLOAT_EQ(userData.getHeight(), 8.0f);
}

TEST(UserExpectationTest, returnsCurrentHeightOnEof)
{
    UserExpectation userData;
    {
        std::istringstream input("10\n");
        std::streambuf* origCin = std::cin.rdbuf();
        std::cin.rdbuf(input.rdbuf());
        userData.setHeight(0);
        std::cin.rdbuf(origCin);
    }

    std::istringstream emptyInput("");
    std::streambuf* origCin = std::cin.rdbuf();
    std::cin.rdbuf(emptyInput.rdbuf());

    float result = userData.setHeight(0);

    std::cin.rdbuf(origCin);
    EXPECT_FLOAT_EQ(result, 10.0f);
    EXPECT_FLOAT_EQ(userData.getHeight(), 10.0f);
}

TEST(UserExpectationTest, exposesDefaultGrassHeightBounds)
{
    UserExpectation userData;

    EXPECT_FLOAT_EQ(userData.getMinGrassHeight(), 5.0f);
    EXPECT_FLOAT_EQ(userData.getMaxGrassHeight(), 15.0f);
}
