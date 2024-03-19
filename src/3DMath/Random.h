#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include "glm/glm.hpp"

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
inline int RandomInt(int min = 0, int max = 1)
{
    static std::uniform_int_distribution<int> distribution(min, max);
    static thread_local std::mt19937 generator;
    return distribution(generator);
}


template<typename T>
glm::vec<3, T> RandomInUnitSphere()
{
    glm::vec<3, T> res(RandomNormalReal<T>(), RandomNormalReal<T>(), RandomNormalReal<T>());
    T r = RandomReal<T>();
    return glm::normalize(res) * glm::pow(r, (T)1.0 / (T)3.0);
}
template<typename T>
glm::vec<2, T> RandomInUnitDisk()
{
    while(true)
    {
        glm::vec<2, T> res(RandomNormalReal<T>(), RandomNormalReal<T>());
        if(glm::dot(res, res) >= T(1))
            continue;
        else
            return res;
    }
}
template<typename T>
glm::vec<3, T> RandomOnUnitSphere()
{
    glm::vec<3, T> res(RandomNormalReal<T>(), RandomNormalReal<T>(), RandomNormalReal<T>());
    return glm::normalize(res);
}
}
#endif
