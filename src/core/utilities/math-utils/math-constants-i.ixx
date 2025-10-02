export module mathconstants;

import std;
import types;
import concepts;

export
{
  template<FloatingArithmetic T>
  constexpr T infinity{ std::numeric_limits<T>::infinity() };

  template<FloatingArithmetic T>
  constexpr T epsilonMachine{ std::numeric_limits<T>::epsilon() };

  template<class T>
  constexpr T kSafeNormalizeLen{ T{ 32 } *epsilonMachine<T> };

  float  oneMinusEpsFloat{ std::nextafter(float{1.0}, float{0.0}) };
  double oneMinusEpsDouble{ std::nextafter(1.0, 0.0) };

  constexpr float kPi{ 3.14159265358979323846f };
  constexpr float kInvPi{ 0.31830988618379067154f };
  constexpr float kInv2Pi{ 0.15915494309189533577f };
  constexpr float kInv4Pi{ 0.07957747154594766788f };
  constexpr float kPiOver2{ 1.57079632679489661923f };
  constexpr float kPiOver4{ 0.78539816339744830961f };
  constexpr float kSqrt2{ 1.41421356237309504880f };
}