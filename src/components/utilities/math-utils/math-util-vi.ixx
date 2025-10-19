export module mathutil;

import std;
import concepts;
import types;
import mathconstants;
import mathfp;

export
{
  // Functions

  // Unused
  template <class Pred>
  [[nodiscard]] constexpr Int findInterval(Int n, Pred pred) noexcept
  {
    Int first{}; Int len{ n };

    while (len > 0)
    {
      Int half{ len >> 1 }; Int mid{ first + half };

      if (pred(mid))
      {
        first = mid + 1; len -= half + 1;
      }
      else len = half;
    }

    return clamp(first - 1, 0, n - 2);
  }

  constexpr Int primeTableSize{ 1000 };

  [[nodiscard]] constexpr bool isPrime(Int n)
  {
    if (n < 2) return false;
    if ((n & 1) == 0) return n == 2;

    for (Int d{ 3 }; d * d <= n; d += 2)
    {
      if (n % d == 0) return false;
    }

    return true;
  }

  [[nodiscard]] constexpr Int nextPrime(Int x)
  {
    for (Int v{ x + 1 };; ++v)
    {
      if (isPrime(v)) return v;
    }
  }

  constexpr std::array<Int, primeTableSize> generatePrimes()
  {
    std::array<Int, primeTableSize> a{};

    Int v{ 2 }; Int i{};

    while (i < primeTableSize)
    {
      if (isPrime(v))
      {
        a[i++] = v;
      }
      ++v;
    };

    return a;
  }

  constexpr std::array<Int, primeTableSize> primes{ generatePrimes() };
};
