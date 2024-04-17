
#include "Tools.hpp"

/// @brief take two signals and return the crossfade
		/// @param dry dry signal
		/// @param wet wet signal
		/// @param parameter 0 full dry / 1 full wet
		/// @return 
		float linearCrossfade(float dry, float wet, float parameter){
			return (dry * (1-parameter)) + (wet * parameter);
		}

/// @brief take two signals and return the crossfade
		/// @param dry dry signal
		/// @param wet wet signal
		/// @param parameter 0 full dry / 1 full wet
		/// @return 
		float equalGainCrossfade(float dry, float wet, float parameter){
                    float half_pi = 1.57;
                    float dry_coef = std::cosf((1 - parameter) * half_pi);
                    float wet_coef = std::cosf(parameter * half_pi);
                    return (dry * dry_coef) + (wet * wet_coef);
                }