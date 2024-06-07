export module h_miscutilities;

import <limits>;
import <cmath>;
import <numbers>;
import <random>;
import <print>;
import <intrin.h>;
import <immintrin.h>;

export
{
    // Checks.
    
    // Build check.
    constexpr bool H_IS_DEBUG_BUILD =
    #ifdef NDEBUG
        false;
    #else
        true;
    #endif

    bool HIsAVX2Enabled()
    {
        int info[4]{};
        __cpuidex(info, 0, 0);
        if (info[0] >= 7)
        {
            __cpuidex(info, 7, 0);
            
            return (info[1] & (1 << 5)) != 0;
        }
        return false;
    }
};