#ifndef QUATERNION_H
#define QUATERNION_H

#include "Matrix.h"
#include "Vector3.h"
#include <string>


namespace math
{
template<typename T>
struct quat
{
    quat()
    {
        w = dynamic_cast<T>(1);
        x = dynamic_cast<T>(0);
        y = dynamic_cast<T>(0);
        z = dynamic_cast<T>(0);
    }
    quat(T _w, T _x, T _y, T _z)
    {
        w = _w;
        x = _x;
        y = _y;
        z = _z;
    }
    quat(T angle, vec<3, T> axis)
    {
        const float sinHalfAngle = std::sin(angle / 2);
        w                        = std::cos(angle / 2);
        x                        = axis.x * sinHalfAngle;
        y                        = axis.y * sinHalfAngle;
        z                        = axis.z * sinHalfAngle;
    }

    T w, x, y, z;

    template<typename U>
    quat<T>& operator+=(const quat<U>& q);
    template<typename U>
    quat<T>& operator-=(const quat<U>& q);
    template<typename U>
    quat<T>& operator*=(const quat<U>& q);
};

template<typename T>
template<typename U>
quat<T>& quat<T>::operator+=(quat<U> const& q)
{
    this->w += static_cast<T>(q.w);
    this->x += static_cast<T>(q.x);
    this->y += static_cast<T>(q.y);
    this->z += static_cast<T>(q.z);
    return *this;
}

template<typename T>
template<typename U>
quat<T>& quat<T>::operator-=(quat<U> const& q)
{
    this->w -= static_cast<T>(q.w);
    this->x -= static_cast<T>(q.x);
    this->y -= static_cast<T>(q.y);
    this->z -= static_cast<T>(q.z);
    return *this;
}

template<typename T>
template<typename U>
quat<T>& quat<T>::operator*=(const quat<U>& q)
{
    quat<T> q1 = *this;

    /*[w1+ix1+jy1+kz1]* [w2 + ix2 + jy2 + kz2] =
    w1w2 + w1ix2 + w1jy2 + w1kz2 +            w1w2 + i * w1x2 + j * w1y2 + k *
    w1z2 +            w1w2-x1x2-y1y2-z1z2 ix1w2 + ix1ix2 + ix1jy2 + ix1kz2 +    =
    i x1w2 + -1 * x1x2 + k * x1y2 + -j * x1z2 +    =   i(w2x1 + w1x2 + y1z2 -
    z1y2) jy1w2 + jy1ix2 + jy1jy2 + jy1kz2 +        j y1w2 + -k * y1x2 + -1 * y1y2
    + i * y1z2 +        j(w2y1 + w1y2 - x1z2 + z1x2) kz1w2 + kz1ix2 + kz1jy2 +
    kz1kz2          k z1w2 + j * z1x2 + -i * z1y2 + -1 * z1z2          k(w2z1 +
    w1z2 + x1y2 - y1x2)*/

    this->w = q1.w * q.w - (q1.x * q.x + q1.y * q.y + q1.z * q.z);
    this->x = q.w * q1.x + q1.w * q.x + q1.y * q.z - q1.z * q.y;
    this->y = q.w * q1.y + q1.w * q.y + q1.z * q.x - q1.x * q.z;
    this->z = q.w * q1.z + q1.w * q.z + q1.x * q.y - q1.y * q.x;
    return *this;
}

template<typename T>
quat<T> operator+(const quat<T>& q1, const quat<T>& q2)
{
    return quat<T>(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
}

template<typename T>
quat<T> operator-(const quat<T>& q1, const quat<T>& q2)
{
    return quat<T>(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
}

template<typename T>
quat<T> operator*(const quat<T>& q1, const quat<T>& q2)
{
    return quat<T>(q1) *= q2;
}

template<typename T>
quat<T> operator*(const quat<T>& q, const vec<3, T>& v)
{
    quat<T> vecq(0, v.x, v.y, v.z);

    return q * vecq;
}
template<typename T>
quat<T> operator/(const quat<T>& q, T scalar)
{
    return quat<T>(q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar);
}

template<typename T>
mat<T> toMat4(const quat<T>& q)
{
    T qxx(q.x * q.x);
    T qyy(q.y * q.y);
    T qzz(q.z * q.z);
    T qxz(q.x * q.z);
    T qxy(q.x * q.y);
    T qyz(q.y * q.z);
    T qwx(q.w * q.x);
    T qwy(q.w * q.y);
    T qwz(q.w * q.z);

    mat<T> result;

    result[0][0] = T(1) - T(2) * (qyy + qzz);
    result[0][1] = T(2) * (qxy + qwz);
    result[0][2] = T(2) * (qxz - qwy);

    result[1][0] = T(2) * (qxy - qwz);
    result[1][1] = T(1) - T(2) * (qxx + qzz);
    result[1][2] = T(2) * (qyz + qwx);

    result[2][0] = T(2) * (qxz + qwy);
    result[2][1] = T(2) * (qyz - qwx);
    result[2][2] = T(1) - T(2) * (qxx + qyy);

    return result;
}

template<typename T>
T dot(const quat<T>& q1, const quat<T>& q2)
{
    return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

template<typename T>
T lengthSq(const quat<T>& q)
{
    return dot(q, q);
}
template<typename T>
T length(const quat<T>& q)
{
    return sqrt(dot(q, q));
}

template<typename T>
quat<T> normalize(const quat<T>& q)
{
    return q / length(q);
}

template<typename T>
quat<T> conjugate(const quat<T>& q)
{
    return quat<T>(q.w, -(q.x), -(q.y), -(q.z));
}

template<typename T>
quat<T> inverse(const quat<T>& q)
{
    return conjugate(q) / lengthSq(q);
}

template<typename T>
std::string ToString(const quat<T>& q)
{
    std::string result = "Quaternion {" + std::to_string(q.w) + "; " + std::to_string(q.x) + "; " + std::to_string(q.y) + "; " + std::to_string(q.z) + "}";
    return result;
}
}  // namespace math
#endif
