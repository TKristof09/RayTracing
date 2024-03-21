#pragma once
#include <glm/glm.hpp>

class ONB
{
public:
    ONB(const glm::vec3& w)
    {
        axis[2] = glm::normalize(w);

        glm::vec3 a;
        if(glm::abs(w.x) > 0.9)
            a = glm::vec3(0, 1, 0);
        else
            a = glm::vec3(1, 0, 0);
        axis[1] = glm::normalize(glm::cross(axis[2], a));
        axis[0] = glm::cross(axis[2], axis[1]);
    }

    glm::vec3 operator[](int i) const { return axis[i]; }

    glm::vec3 Local(const glm::vec3& a) const
    {
        return a.x * axis[0] + a.y * axis[1] + a.z * axis[2];
    }

private:
    glm::vec3 axis[3];
};
