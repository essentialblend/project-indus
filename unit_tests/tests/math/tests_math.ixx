export module tests_math;

import <gtest/gtest.h>;
import h_mathutilities;
import tuple2;
import tuple3;

export 
{
    template <typename T>
    class Vec3 : public Tuple3<Vec3, T> {
    public:
        using Tuple3<Vec3, T>::Tuple3; // Inherit constructors from Tuple3
        using value_type = T; // Define value_type
        using Tuple3<Vec3, T>::operator+;
        static constexpr std::size_t m_numDimensions = 3; // Define m_numDimensions

        // Constructor to handle initializer list
        Vec3(std::initializer_list<T> list) : Tuple3<Vec3, T>(*(list.begin()), *(list.begin() + 1), *(list.begin() + 2)) {
            if (list.size() != 3) {
                throw std::invalid_argument("Initializer list must have exactly 3 elements");
            }
        }

        constexpr auto operator<=>(const Vec3&) const = default;

        // Begin and end methods
        auto begin() { return &this->m_x; }
        auto end() { return &this->m_x + m_numDimensions; }

        auto begin() const { return &this->m_x; }
        auto end() const { return &this->m_x + m_numDimensions; }
    };

    template <typename T>
    class Vec2 : public Tuple2<Vec2, T> {
    public:
        using Tuple2<Vec2, T>::Tuple2; // Inherit constructors from Tuple2
        using value_type = T; // Define value_type
        using Tuple2<Vec2, T>::operator+;
        static constexpr std::size_t m_numDimensions = 2; // Define m_numDimensions

        // Constructor to handle initializer list
        Vec2(std::initializer_list<T> list) : Tuple2<Vec2, T>(*(list.begin()), *(list.begin() + 1)) {
            if (list.size() != 2) {
                throw std::invalid_argument("Initializer list must have exactly 2 elements");
            }
        }

        constexpr auto operator<=>(const Vec2&) const = default;

        // Begin and end methods
        auto begin() { return &this->m_x; }
        auto end() { return &this->m_x + m_numDimensions; }

        auto begin() const { return &this->m_x; }
        auto end() const { return &this->m_x + m_numDimensions; }
    };

    TEST(Tuple2Tests, Initialization) {
        Vec2<Float> v{ 1.0, 2.0 };
        EXPECT_NEAR(v[0], 1.0, 1e-9);
        EXPECT_NEAR(v[1], 2.0, 1e-9);
    }

    TEST(Tuple3Tests, Initialization) {
        Vec3<Float> v{ 1.0, 2.0, 3.0 };
        EXPECT_NEAR(v[0], 1.0, 1e-9);
        EXPECT_NEAR(v[1], 2.0, 1e-9);
        EXPECT_NEAR(v[2], 3.0, 1e-9);
    }

    TEST(Tuple2Tests, ArithmeticOperations) {
        Vec2<Float> v1{ 1.0, 2.0 };
        Vec2<Float> v2{ 3.0, 4.0 };

        auto v3 = v1 + v2;
        EXPECT_NEAR(v3[0], 4.0, 1e-9);
        EXPECT_NEAR(v3[1], 6.0, 1e-9);

        v1 += v2;
        EXPECT_NEAR(v1[0], 4.0, 1e-9);
        EXPECT_NEAR(v1[1], 6.0, 1e-9);
    }

    TEST(Tuple3Tests, ArithmeticOperations) {
        Vec3<Float> v1{ 1.0, 2.0, 3.0 };
        Vec3<Float> v2{ 4.0, 5.0, 6.0 };

        auto v3 = v1 + v2;
        EXPECT_NEAR(v3[0], 5.0, 1e-9);
        EXPECT_NEAR(v3[1], 7.0, 1e-9);
        EXPECT_NEAR(v3[2], 9.0, 1e-9);

        v1 += v2;
        EXPECT_NEAR(v1[0], 5.0, 1e-9);
        EXPECT_NEAR(v1[1], 7.0, 1e-9);
        EXPECT_NEAR(v1[2], 9.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HComputeDot) {
        Vec3<Float> v1{ 1.0, 2.0, 3.0 };
        Vec3<Float> v2{ 4.0, 5.0, 6.0 };

        auto result = HComputeDot(v1, v2);
        EXPECT_NEAR(result, 32.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HComputeDistance) {
        Vec3<Float> v1{ 1.0, 2.0, 3.0 };
        Vec3<Float> v2{ 4.0, 6.0, 3.0 };

        auto result = HComputeDistance(v1, v2);
        EXPECT_NEAR(result, 5.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HComputeLength) {
        Vec3<Float> v{ 1.0, 2.0, 3.0 };

        auto result = HComputeLength(v);
        EXPECT_NEAR(result, std::sqrt(14.0), 1e-9);
    }

    TEST(MathUtilitiesTests, HPermute) 
    {
        Vec3<Float> v{ 1.0, 2.0, 3.0 };
        auto result = HPermute<Vec3<Float>, 2, 1, 0>(v);
        EXPECT_NEAR(result[0], 3.0, 1e-9);
        EXPECT_NEAR(result[1], 2.0, 1e-9);
        EXPECT_NEAR(result[2], 1.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HComputeAbs) {
        Vec3<Float> v{ -1.0, -2.0, -3.0 };
        auto result = HComputeAbs(v);
        EXPECT_NEAR(result[0], 1.0, 1e-9);
        EXPECT_NEAR(result[1], 2.0, 1e-9);
        EXPECT_NEAR(result[2], 3.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HComputeCeil) {
        Vec3<Float> v{ 1.1, 2.5, 3.9 };
        auto result = HComputeCeil(v);
        EXPECT_NEAR(result[0], 2.0, 1e-9);
        EXPECT_NEAR(result[1], 3.0, 1e-9);
        EXPECT_NEAR(result[2], 4.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HComputeFloor) {
        Vec3<Float> v{ 1.9, 2.5, 3.1 };
        auto result = HComputeFloor(v);
        EXPECT_NEAR(result[0], 1.0, 1e-9);
        EXPECT_NEAR(result[1], 2.0, 1e-9);
        EXPECT_NEAR(result[2], 3.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HLerp) {
        Vec3<Float> v1{ 1.0, 2.0, 3.0 };
        Vec3<Float> v2{ 4.0, 5.0, 6.0 };
        auto result = HLerp(0.5, v1, v2);
        EXPECT_NEAR(result[0], 2.5, 1e-9);
        EXPECT_NEAR(result[1], 3.5, 1e-9);
        EXPECT_NEAR(result[2], 4.5, 1e-9);
    }

    TEST(MathUtilitiesTests, HProd) {
        Vec3<Float> v{ 2.0, 3.0, 4.0 };
        auto result = HProd(v);
        EXPECT_NEAR(result, 24.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HGetMinComponentValue) {
        Vec3<Float> v{ 3.0, 1.0, 2.0 };
        auto result = HGetMinComponentValue<Vec3<Float>, Float>(v);
        EXPECT_NEAR(result, 1.0, 1e-9);
    }

    TEST(MathUtilitiesTests, HGetMaxComponentValue) {
        Vec3<Float> v{ 3.0, 1.0, 2.0 };
        auto result = HGetMaxComponentValue<Vec3<Float>, Float>(v);
        EXPECT_NEAR(result, 3.0, 1e-9);
    }

	TEST(MathUtilitiesTests, HGetMinComponentIndex) {
		Vec3<Float> v{ 3.0, 1.0, 2.0 };
		auto result = HGetMinComponentIndex(v);
		EXPECT_EQ(result, 1);
	}

	TEST(MathUtilitiesTests, HGetMaxComponentIndex) {
		Vec3<Float> v{ 3.0, 1.0, 2.0 };
		auto result = HGetMaxComponentIndex(v);
		EXPECT_EQ(result, 0);
	}
};