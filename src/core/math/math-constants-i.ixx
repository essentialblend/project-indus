export module indus.core.math.constants.i;

import indus.core.concepts;

export
{
  template<FloatingArithmetic T>
  constexpr T infinity{ std::numeric_limits<T>::infinity() };

  template<FloatingArithmetic T>
  constexpr T epsilonMachine{ std::numeric_limits<T>::epsilon() };

  template<class T>
  constexpr T kSafeNormalizeLen{ T{ 32 } * epsilonMachine<T> };

  template<FloatingArithmetic T>
  T oneMinusEps() { return std::nextafter(T{ 1 }, T{ 0 }); }

  template<FloatingArithmetic T>
  constexpr T kPi_v{ static_cast<T>(3.14159265358979323846264338327950288L) };

  template<FloatingArithmetic T>
  constexpr T kInvPi_v{ T{ 1 } / kPi_v<T> };

  template<FloatingArithmetic T>
  constexpr T kInv2Pi_v{ T{ 1 } / (T{ 2 } * kPi_v<T>) };

  template<FloatingArithmetic T>
  constexpr T kInv4Pi_v{ T{ 1 } / (T{ 4 } * kPi_v<T>) };

  template<FloatingArithmetic T>
  constexpr T kPiOver2_v{ kPi_v<T> * T{ 0.5 } };

  template<FloatingArithmetic T>
  constexpr T kPiOver4_v{ kPi_v<T> * T{ 0.25 } };
}