export module indus.core.math.float_constants;

import indus.core.types;
import indus.core.math.constants.i;

export
{
  constexpr Float kPi{ kPi_v<Float> };
  constexpr Float kInvPi{ kInvPi_v<Float> };
  constexpr Float kInv2Pi{ kInv2Pi_v<Float> };
  constexpr Float kInv4Pi{ kInv4Pi_v<Float> };
  constexpr Float kPiOver2{ kPiOver2_v<Float> };
  constexpr Float kPiOver4{ kPiOver4_v<Float> };

  const Float oneMinusEpsFloat{ oneMinusEps<Float>() };
  const Float64 oneMinusEpsDouble{ oneMinusEps<Float64>() };

  constexpr Float kInfinity{ infinity<Float> };
  constexpr Float64 kInfinityDouble{ infinity<Float64> };

  constexpr Float kEpsilon{ epsilonMachine<Float> };
  constexpr Float64 kEpsilonDouble{ epsilonMachine<Float64> };

  constexpr Float kSafeNormalizeLenFloat{ kSafeNormalizeLen<Float> };
}