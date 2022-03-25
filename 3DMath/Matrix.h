#ifndef MATRIX_H
#define MATRIX_H

#include <string>

namespace math
{

    template<typename T>
    struct mat
    {
        mat()
        {   //might be bad layout but I think it is good
            m_values[0][0] = 1; m_values[1][0] = 0; m_values[2][0] = 0; m_values[3][0] = 0;
            m_values[0][1] = 0; m_values[1][1] = 1; m_values[2][1] = 0; m_values[3][1] = 0;
            m_values[0][2] = 0; m_values[1][2] = 0; m_values[2][2] = 1; m_values[3][2] = 0;
            m_values[0][3] = 0; m_values[1][3] = 0; m_values[2][3] = 0; m_values[3][3] = 1;
        };

        mat(T s)
        {

            for(int column = 0; column < 4; column++)
            {
                for(int row = 0; row < 4; row++)
                {
                    if (row == column)
                        m_values[column][row] = s;
                    else
                        m_values[column][row] = 0;
                }
            }
        }
        const T* operator[](unsigned int i) const
        {
            return m_values[i];
        }
        T* operator[](unsigned int i)
        {
            return m_values[i];
        }

    private:
        T m_values[4][4];
    };

    template<typename T>
    mat<T> operator*(const mat<T>& m1, const mat<T>& m2)
    {
        mat<T> result;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                result[column][row] = T(0);
                for (int k = 0; k < 4; k++)
                {
                    result[column][row] += m1[k][row] * m2[column][k];
                }
            }
        }

        return result;
    }

    template<typename T>
    mat<T> operator*(const mat<T>& m, T scalar)
    {
        mat<T> result;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                result[column][row] = m[column][row] * scalar;
            }
        }
        return result;
    }
    template<typename T>
    mat<T> operator*(T scalar, const mat<T>& m)
    {
        mat<T> result;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                result[column][row] = m[column][row] * scalar;
            }
        }
        return result;
    }

    template<typename T>
    mat<T> operator+(const mat<T>& m1, const mat<T>& m2)
    {
        mat<T> result;
        for(int column = 0; column < 4; column++)
        {
            for(int row = 0; row < 4; row++)
            {
                result[column][row] = m1[column][row] + m2[column][row];
            }
        }
        return result;
    }

    template<typename T>
    mat<T> operator+(const mat<T>& m, T scalar)
    {
        mat<T> result;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                result[column][row] = m[column][row] + scalar;
            }
        }
        return result;
    }

    template<typename T>
    mat<T> translate(const vec<3, T>& v)
    {
        mat<T> result;
        result[3][0] = v.x;
        result[3][1] = v.y;
        result[3][2] = v.z;

        return result;
    }

    template<typename T>
    mat<T> scale(const vec<3, T>& v)
    {
        mat<T> result;
        result[0][0] = v.x;
        result[1][1] = v.y;
        result[2][2] = v.z;
        return result;
    }

    template<typename T>
    mat<T> transpose(const mat<T>& m)
    {
        mat<T> result;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                result[column][row] = m[row][column];
            }
        }
        return result;
    }

    template<typename T>
    mat<T> perspective(T fov, T aspectRatio, T zNear, T zFar)
    {
        mat<T> result(static_cast<T>(0));
        const float tanHalfFOV = tan(fov / 2.0f);

        result[0][0] = static_cast<T>(1) / (aspectRatio * tanHalfFOV);
        result[1][1] = static_cast<T>(1) / (tanHalfFOV);
        result[2][3] = -static_cast<T>(1);

        result[2][2] = -(zFar + zNear) / (zFar - zNear);
        result[3][2] = -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

        return result;
    }

//    template<typename T>
//    double determinant(const mat<T>& m)
//    {
//        double result = 0;
//        //bad
//        for(int column = 0; column < 4; column++)
//        {
//            for(int column2 = 0; column2 < 4; column2++)
//            {
//                if(column == column2)
//                    continue;
//                for(int row = 1; row < 4; row++)
//                {
//                    
//                    result += (pow(-1, column) * m[column][0] * m[column2][row]);
//                }
//            }
//        }
//        return result;
//    }
//
//    template<typename T>
//    mat<T> inverse(const mat<T>& m)
//    {
//        //m * (1/determinant(m))
//        
//    }

    template<typename T>
    std::string ToString(const mat<T>& m)
    {
        std::string result = "Matrix 4x4 :\n" + std::to_string(m[0][0]) + "; " + std::to_string(m[1][0]) + "; " + std::to_string(m[2][0]) + "; " + std::to_string(m[3][0]) + "\n"
                                              + std::to_string(m[0][1]) + "; " + std::to_string(m[1][1]) + "; " + std::to_string(m[2][1]) + "; " + std::to_string(m[3][1]) + "\n"
                                              + std::to_string(m[0][2]) + "; " + std::to_string(m[1][2]) + "; " + std::to_string(m[2][2]) + "; " + std::to_string(m[3][2]) + "\n"
                                              + std::to_string(m[0][3]) + "; " + std::to_string(m[1][3]) + "; " + std::to_string(m[2][3]) + "; " + std::to_string(m[3][3]);
        
        return result;
    }
}
#endif