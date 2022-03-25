#ifndef VECTOR4_H
#define VECTOR4_H

#include "Matrix.h"

namespace math
{
    template<unsigned int L, typename T> struct vec;

    template<typename T>
    struct vec<4, T>
    {
        vec() {}

        vec(T _s)
        {
            x = _s;
            y = _s;
            z = _s;
            w = _s;
        }

        vec(const vec<3, T>& v, T _w)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = _w;
        }

        vec(T _x, T _y, T _z, T _w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;

        }
        union
        {
            struct { T x, y, z, w; };
            struct { T r, g, b, a; };
        };
        template<typename U>
        vec<4, T>&  operator+=(const vec<4, U>& v);
        template<typename U>
        vec<4, T>&  operator-=(const vec<4, U>& v);
        template<typename U>
        vec<4, T>&  operator*=(const vec<4, U>& v);
    };

    template<typename T>
    template<typename U>
    vec<4, T>& vec<4, T>::operator+=(const vec<4, U>& v)
    {
        this->x += static_cast<T>(v.x);
        this->y += static_cast<T>(v.y);
        this->z += static_cast<T>(v.z);
        this->w += static_cast<T>(v.w);
        return *this;
    }

    template<typename T>
    template<typename U>
    vec<4, T>& vec<4, T>::operator-=(const vec<4, U>& v)
    {
        this->x -= static_cast<T>(v.x);
        this->y -= static_cast<T>(v.y);
        this->z -= static_cast<T>(v.z);
        this->w -= static_cast<T>(v.w);
        return *this;
    }

    template<typename T>
    template<typename U>
    vec<4, T>& vec<4, T>::operator*=(const vec<4, U>& v)
    {
        this->x *= static_cast<T>(v.x);
        this->y *= static_cast<T>(v.y);
        this->z *= static_cast<T>(v.z);
        this->w *= static_cast<T>(v.w);
        return *this;
    }

    template<typename T>
    vec<4, T> operator+(const vec<4, T>& v1, const vec<4, T>& v2)
    {
        return vec<4, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    template<typename T>
    vec<4, T> operator-(const vec<4, T>& v1, const vec<4, T>& v2)
    {
        return vec<4, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }

    template<typename T>
    vec<4, T> operator*(const vec<4, T>& v1, const vec<4, T>& v2)
    {
        return vec<4, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
    }

    template<typename T>
    vec<4, T> operator*(const mat<T>& m, const vec<4, T>& v)
    {
        vec<4, T> result;
        result.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w;
        result.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w;
        result.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w;
        result.w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w;
        return result;
    }
    template<typename T>
    vec<4, T> operator/(const vec<4, T>& v1, T scalar)
    {
        return vec<4, T>(v1.x / scalar, v1.y / scalar, v1.z / scalar, v1.w / scalar);
    }
    template<typename T>
    T compMax(const vec<4, T>& v)
    {
        return max(v.x, max(v.y, max(v.z, v.w)));
    }

}
#endif