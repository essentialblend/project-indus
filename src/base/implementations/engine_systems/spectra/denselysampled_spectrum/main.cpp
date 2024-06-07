import core_constructs;

import <gtest/gtest.h>;

import h_mathutilities;
import tuple2;

import <memory>;
import <vector>;
import <print>;
import <array>;
import <cmath>;
import <iostream>;
import <cassert>;

template <typename T>
class Vec2 : public Tuple2<Vec2, T> {
public:
    using Tuple2<Vec2, T>::Tuple2;

    // Constructor to handle initializer list
    Vec2(std::initializer_list<T> list) : Tuple2<Vec2, T>(*list.begin(), *(list.begin() + 1)) 
    {
        if (list.size() != 2) 
        {
            throw std::invalid_argument("Initializer list must have exactly 2 elements");
        }
    }
};

// Single Test Case for Default Constructor
TEST(Vec2Test, ConstructorDefault) {
    Vec2<float> v1;
    EXPECT_EQ(v1[0], 0.0f);
    EXPECT_EQ(v1[1], 0.0f);
}

TEST(Vec2Test, ConstructorWithValues) {
    Vec2<float> v2(1.5f, 2.5f);
    EXPECT_EQ(v2[0], 1.5f);
    EXPECT_EQ(v2[1], 2.5f);
}

TEST(Vec2Test, ConstructorWithInitializerList) {
    Vec2<float> v3{ 1.5f, 2.5f };
    EXPECT_EQ(v3[0], 1.5f);
    EXPECT_EQ(v3[1], 2.5f);
}

TEST(Vec2Test, Negation) {
    Vec2<float> v(1.5f, -2.5f);
    Vec2<float> neg_v = -v;
    EXPECT_EQ(neg_v[0], -1.5f);
    EXPECT_EQ(neg_v[1], 2.5f);
}

TEST(Vec2Test, Addition) {
    Vec2<float> v1(1.5f, 2.5f);
    Vec2<float> v2(2.5f, 1.5f);
    Vec2<float> result = v1 + v2;
    EXPECT_EQ(result[0], 4.0f);
    EXPECT_EQ(result[1], 4.0f);
}

TEST(Vec2Test, Multiplication) {
    Vec2<float> v(1.5f, 2.5f);
    Vec2<float> result = v * 2.0f;
    EXPECT_EQ(result[0], 3.0f);
    EXPECT_EQ(result[1], 5.0f);
}

TEST(Vec2Test, Division) {
    Vec2<float> v(1.5f, 2.5f);
    Vec2<float> result = v / 2.0f;
    EXPECT_EQ(result[0], 0.75f);
    EXPECT_EQ(result[1], 1.25f);
}

TEST(Vec2Test, ToString) {
    Vec2<float> v(1.5f, 2.5f);
    std::string expected = "Tuple2(1.5, 2.5)";
    EXPECT_EQ(v.toString(), expected);
}

TEST(Vec2Test, HasNaN) {
    Vec2<float> v1(1.5f, std::numeric_limits<float>::quiet_NaN());
    EXPECT_TRUE(v1.hasNaN());
    Vec2<float> v2(std::numeric_limits<float>::quiet_NaN(), 2.5f);
    EXPECT_TRUE(v2.hasNaN());
    Vec2<float> v3(1.5f, 2.5f);
    EXPECT_FALSE(v3.hasNaN());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}