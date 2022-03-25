#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include "Vector.h"

namespace math
{
	template<typename T>
	T RandomReal(T min = T(0.0), T max = T(1.0))
	{
		static std::uniform_real_distribution<T> distribution(min, max);
		static std::mt19937 generator;
		return distribution(generator);
	}


	template<typename T>
	vec<3,T> RandomInUnitSphere()
	{
		while(true)
		{
			vec<3,T> res(RandomReal<T>(), RandomReal<T>(),RandomReal<T>());
			if(math::lengthSq(res) >= T(1))
				continue;
			else
				return res;
		}
	}
	template<typename T>
	vec<3,T> RandomOnUnitSphere()
	{
		return math::normalize(RandomInUnitSphere<T>());
	}


}
#endif
