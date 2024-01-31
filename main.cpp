#include <filesystem>
#include <iostream>
#include <stdint.h>
#include <execution>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "3DMath/3DMath.h"
#include "3DMath/Random.h"
#include "AARect.h"
#include "BVH.h"
#include "Camera.h"
#include "HittableList.h"
#include "Material.h"
#include "Ray.h"
#include "Sphere.h"
#include "Texture.h"

math::Vec3d RayColor(const Ray& r, const math::Vec3d& background,
                     const Hittable& world, int depth)
{
    if(depth <= 0)
        return math::Vec3d(0);
    HitRecord rec;

    if(world.Hit(r, 0.001, std::numeric_limits<double>::infinity(), rec))
    {
        // Hemispherical scattering
        // math::Vec3d v = math::RandomInUnitSphere<double>();
        // math::Vec3d target = rec.point + (math::dot(v, rec.normal) > 0.0 ? v :
        // -v);

        // Lambertian scattering
        // math::Vec3d target = rec.point + rec.normal +
        // math::RandomOnUnitSphere<double>();

        Ray scattered;
        math::Vec3d attenuation;
        math::Vec3d emitted = rec.material->Emitted(rec.uv, rec.point);
        if(rec.material->Scatter(r, rec, attenuation, scattered))
            return emitted + attenuation * RayColor(scattered, background, world, depth - 1);
        else
            return emitted;

        // return 0.5 * RayColor(Ray(rec.point, target - rec.point), world, depth -
        // 1);
    }

    // background
    return background;
}

std::string GetCurrentFilename(const std::string& base,
                               const std::string& ext)
{
    std::filesystem::path p = std::filesystem::current_path() / "images";

    int i = 0;
    for(const auto& entry : std::filesystem::directory_iterator(p))
    {
        if(entry.path().filename().string().find(base) == 0 && entry.path().extension().string() == ext)
            i++;
    }

    return "images/" + base + std::to_string(i) + ext;
}
HittableList RandomScene()
{
    // Materials
    auto sphereMat = std::make_shared<Lambertian>(math::Vec3d(0.8, 0, 0.2));
    auto groundMat = std::make_shared<Lambertian>(
        std::make_shared<CheckerTexture>(math::Vec3d(0), math::Vec3d(1)));
    auto sphereLeft = std::make_shared<Dielectric>(1.5);
    auto metalRight = std::make_shared<Metal>(math::Vec3d(0.8, 0.6, 0.2), 1);
    // world
    HittableList world;

    for(int a = -11; a < 11; a++)
    {
        for(int b = -11; b < 11; b++)
        {
            auto matChoice = math::RandomReal<double>();
            math::Vec3d center(a + 0.9 * math::RandomReal<double>(), 0.2,
                               b + 0.9 * math::RandomReal<double>());
            if(math::lengthSq(center - math::Vec3d(4, 0.2, 0)) > 0.9 * 0.9)
            {
                std::shared_ptr<Material> mat;
                if(matChoice < 0.8)
                {
                    // diffuse
                    auto albedo = math::RandomInUnitSphere<double>() * math::RandomInUnitSphere<double>();  // random color
                    mat         = std::make_shared<Lambertian>(albedo);
                }
                else if(matChoice < 0.95)
                {
                    // metal
                    auto albedo = math::Vec3d(math::RandomReal<double>(0.5, 1.0),
                                              math::RandomReal<double>(0.5, 1.0),
                                              math::RandomReal<double>(0.5, 1.0));
                    auto fuzz   = math::RandomReal<double>(0, 0.5);
                    mat         = std::make_shared<Metal>(albedo, fuzz);
                }
                else
                {
                    // glass
                    mat = std::make_shared<Dielectric>(1.5);
                }
                world.Add(std::make_shared<Sphere>(center, 0.2, mat));
            }
        }
    }
    auto mat1 = std::make_shared<Dielectric>(1.5);
    world.Add(std::make_shared<Sphere>(math::Vec3d(0, 1, 0), 1, mat1));

    auto mat2 = std::make_shared<Lambertian>(math::Vec3d(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(math::Vec3d(-4, 1, 0), 1, mat2));

    auto mat3 = std::make_shared<Metal>(math::Vec3d(0.7, 0.6, 0.5), 0);
    world.Add(std::make_shared<Sphere>(math::Vec3d(4, 1, 0), 1, mat3));
    // Render into a PPM image
    //

    BVHNode worldBVH(world);

    HittableList objects;
    objects.Add(std::make_shared<BVHNode>(worldBVH));
    objects.Add(std::make_shared<Sphere>(math::Vec3d(0, -1000, 0), 1000,
                                         groundMat));  // "ground"

    return objects;
}

HittableList Earth()
{
    auto texture = std::make_shared<ImageTexture>("earthmap.jpg");
    auto mat     = std::make_shared<Lambertian>(texture);
    auto globe   = std::make_shared<Sphere>(math::Vec3d(0), 2, mat);

    HittableList objects;
    objects.Add(globe);
    return objects;
}
HittableList EmissionScene()
{
    auto texture = std::make_shared<ImageTexture>("earthmap.jpg");
    auto mat1    = std::make_shared<Lambertian>(texture);
    auto mat2    = std::make_shared<Emissive>(4.0 * math::Vec3d(1, 1, 1));

    auto sphere1 = std::make_shared<Sphere>(math::Vec3d(0, 2, 0), 2.0, mat1);
    auto light   = std::make_shared<XY_Rect>(math::Vec2d(3, 1), math::Vec3d(5, 3), -2, mat2);

    HittableList objects;
    objects.Add(sphere1);
    objects.Add(light);

    auto groundMat = std::make_shared<Lambertian>(
        std::make_shared<SolidColor>(math::Vec3d(1, 1, 0)));
    objects.Add(std::make_shared<Sphere>(math::Vec3d(0, -1000, 0), 1000,
                                         groundMat));  // "ground"
    // objects.Add(std::make_shared<XY_Rect>(math::Vec2d(-10,-10), math::Vec3d(10,
    // 10), -5, groundMat));
    return objects;
}
HittableList CornellBox()
{
    HittableList objects;

    auto red   = std::make_shared<Lambertian>(math::Vec3d(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(math::Vec3d(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(math::Vec3d(.12, .45, .15));
    auto light = std::make_shared<Emissive>(15.0 * math::Vec3d(1));

    objects.Add(std::make_shared<YZ_Rect>(math::Vec2d(0, 0),
                                          math::Vec2d(555, 555), 555, green));
    objects.Add(std::make_shared<YZ_Rect>(math::Vec2d(0, 0),
                                          math::Vec2d(555, 555), 0, red));
    objects.Add(std::make_shared<XZ_Rect>(math::Vec2d(213, 227),
                                          math::Vec2d(343, 332), 554, light));
    objects.Add(std::make_shared<XZ_Rect>(math::Vec2d(0, 0),
                                          math::Vec2d(555, 555), 0, white));
    objects.Add(std::make_shared<XZ_Rect>(math::Vec2d(0, 0),
                                          math::Vec2d(555, 555), 555, white));
    objects.Add(std::make_shared<XY_Rect>(math::Vec2d(0, 0),
                                          math::Vec2d(555, 555), 555, white));

    return objects;
}
int main()
{
    // Scene
    HittableList world;
    math::Vec3d camPos;
    math::Vec3d lookAt;
    double vFOV;
    double focusDist;
    double aperture;
    math::Vec3d background;

    switch(4)
    {
    case 1:
        world      = RandomScene();
        camPos     = math::Vec3d(13, 2, 3);
        lookAt     = math::Vec3d(0, 0, 0);
        vFOV       = 20;
        focusDist  = 10;
        aperture   = 0.0;
        background = math::Vec3d(0.7, 0.8, 1.0);
        break;
    case 2:
        world      = Earth();
        camPos     = math::Vec3d(0, 2, 20);
        lookAt     = math::Vec3d(0, 0, 0);
        vFOV       = 20;
        focusDist  = 10;
        aperture   = 0.0;
        background = math::Vec3d(0.7, 0.8, 1.0);
        break;
    case 3:
        world      = EmissionScene();
        camPos     = math::Vec3d(26, 3, 6);
        lookAt     = math::Vec3d(0, 2, 0);
        vFOV       = 20;
        focusDist  = 10;
        aperture   = 0.0;
        background = math::Vec3d(0.01);
        break;
    case 4:
        world      = CornellBox();
        camPos     = math::Vec3d(278, 278, -800);
        lookAt     = math::Vec3d(278, 278, 0);
        vFOV       = 40;
        focusDist  = 10;
        aperture   = 0.0;
        background = math::Vec3d(0.00);
        break;
    }
    const uint32_t numSamples = 1000;
    const int maxDepth        = 50;
    const double aspectRatio  = 3.0 / 2.0;

    const uint32_t imageWidth  = 600;
    const uint32_t imageHeight = static_cast<uint32_t>(imageWidth / aspectRatio);

    // Camera
    Camera cam(camPos, lookAt, math::Vec3d(0, 1, 0), vFOV, aspectRatio, aperture,
               focusDist);

    std::vector<uint8_t> imageData(imageWidth * imageHeight * 3);
    int index = 0;

    std::vector<int> lines;
    lines.reserve(imageHeight);
    for(int i = imageHeight - 1; i >= 0; --i)
        lines.push_back(i);

    std::for_each(std::execution::par, lines.begin(), lines.end(),
                  [=, &imageData](int y)
                  {
                      for(int x = 0; x < imageWidth; ++x)
                      {
                          math::Vec3d color(0);
                          for(int s = 0; s < numSamples; ++s)
                          {
                              double u = (x + math::RandomReal<double>()) / (imageWidth - 1);
                              double v = (y + math::RandomReal<double>()) / (imageHeight - 1);

                              color += RayColor(cam.GetRay(u, v), background, world, maxDepth);
                          }
                          double r = color.r;
                          double g = color.g;
                          double b = color.b;

                          // gamma correct for gamma = 2
                          double scale = 1.0 / numSamples;
                          r            = sqrt(r * scale);
                          g            = sqrt(g * scale);
                          b            = sqrt(b * scale);

                          imageData[((imageHeight - 1 - y) * imageWidth + x) * 3]     = 256 * math::clamp(r, 0.0, 0.999);
                          imageData[((imageHeight - 1 - y) * imageWidth + x) * 3 + 1] = 256 * math::clamp(g, 0.0, 0.999);
                          imageData[((imageHeight - 1 - y) * imageWidth + x) * 3 + 2] = 256 * math::clamp(b, 0.0, 0.999);

                          // WriteColor(std::cout, color, numSamples);
                      }
                  });
    std::string filename = GetCurrentFilename("image", ".png");
    stbi_write_png(filename.c_str(), imageWidth, imageHeight, 3, imageData.data(),
                   imageWidth * 3);
    std::cout << "\nDone.\n"
              << "Wrote to: " << filename << std::endl;
}
