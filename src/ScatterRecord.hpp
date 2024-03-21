#pragma once

#include "Ray.h"
#include "PDF.hpp"
struct ScatterRecord
{
    Ray skipPDFRay;
    std::shared_ptr<PDF> pdf;
    glm::vec3 attenuation;
};
