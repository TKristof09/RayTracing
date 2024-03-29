#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include "glm/glm.hpp"
#include "glm/ext/scalar_constants.hpp"

namespace math
{

template<typename T>
T RandomReal()
{
    static std::uniform_real_distribution<T> distribution(0.0, 1.0);
    static thread_local std::mt19937_64 generator{static_cast<long unsigned int>(time(0))};
    return T(distribution(generator));
}
template<typename T>
T RandomReal(T min, T max)
{
    return RandomReal<T>() * (max - min) + min;
}

template<typename T>
T RandomNormalReal()
{
    static std::normal_distribution<T> distribution;
    static thread_local std::mt19937_64 generator{static_cast<long unsigned int>(time(0))};
    return T(distribution(generator));
}
inline int RandomInt()
{
    static std::uniform_int_distribution<int> distribution;
    static thread_local std::mt19937 generator{static_cast<long unsigned int>(time(0))};
    return distribution(generator);
}
inline int RandomInt(int min, int max)
{
    return RandomInt() % (max - min) + min;
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
    T r     = RandomReal<T>();
    T theta = RandomReal<T>() * glm::pi<T>() * 2.0;
    return glm::vec<2, T>(r * glm::cos(theta), r * glm::sin(theta));
}
template<typename T>
glm::vec<3, T> RandomOnUnitSphere()
{
    glm::vec<3, T> res(RandomNormalReal<T>(), RandomNormalReal<T>(), RandomNormalReal<T>());
    return glm::normalize(res);
}

template<typename T>
glm::vec<3, T> RandomCosineHemisphere()
{
    auto onDisk = RandomInUnitDisk<T>();
    T z         = glm::sqrt(1 - onDisk.x * onDisk.x - onDisk.y * onDisk.y);
    return glm::vec<3, T>(onDisk.x, onDisk.y, z);
}
}  // namespace math
#endif
