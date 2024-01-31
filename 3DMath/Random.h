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
    static thread_local std::mt19937_64 generator;
    return T(distribution(generator));
}

template<typename T>
T RandomNormalReal()
{
    static std::normal_distribution<T> distribution;
    static thread_local std::mt19937_64 generator;
    return T(distribution(generator));
}
int RandomInt(int min = 0, int max = 1)
{
    static std::uniform_int_distribution<int> distribution(min, max);
    static thread_local std::mt19937 generator;
    return distribution(generator);
}


template<typename T>
vec<3, T> RandomInUnitSphere()
{
    vec<3, T> res(RandomNormalReal<T>(), RandomNormalReal<T>(), RandomNormalReal<T>());
    T r = RandomReal<T>();
    return math::normalize(res) * pow(r, 1.0 / 3.0);
}
template<typename T>
vec<2, T> RandomInUnitDisk()
{
    while(true)
    {
        vec<2, T> res(RandomNormalReal<T>(), RandomNormalReal<T>());
        if(math::lengthSq(res) >= T(1))
            continue;
        else
            return res;
    }
}
template<typename T>
vec<3, T> RandomOnUnitSphere()
{
    vec<3, T> res(RandomNormalReal<T>(), RandomNormalReal<T>(), RandomNormalReal<T>());
    return math::normalize(res);
}


}
#endif
