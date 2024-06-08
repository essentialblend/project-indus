import <gtest/gtest.h>;
import <benchmark/benchmark.h>;

import h_mathutilities;
import tuple2;
import tuple3;

import <memory>;
import <vector>;
import <print>;
import <array>;
import <cmath>;
import <iostream>;
import <cassert>;

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

