#include "Box.hpp"
#include "Transformations.hpp"
#include "Volumes.hpp"
#define GLM_FORCE_MESSAGES
#define GLM_ENABLE_EXPERIMENTAL
// #define GLM_FORCE_SIMD_AVX2
#define GLM_FORCE_SIMD_AVX512
#include <filesystem>
#include <iostream>
#include <stdint.h>
#include <execution>
#include "MiniFB_cpp.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "3DMath/Random.h"
#include "Quad.hpp"
#include "BVH.h"
#include "Camera.h"
#include "HittableList.h"
#include "Material.h"
#include "Ray.h"
#include "Sphere.h"
#include "Texture.h"
#include "Allocator.hpp"


#define SHAPE_ALLOCATOR_SIZE    1024 * 512
#define MATERIAL_ALLOCATOR_SIZE 1024 * 1024
LinearAllocator g_shapeAllocator(SHAPE_ALLOCATOR_SIZE);
LinearAllocator g_materialAllocator(MATERIAL_ALLOCATOR_SIZE);


glm::vec3 RayColor(const Ray& r, const glm::vec3& background,
                   const Hittable& world, int depth)
{
    if(depth <= 0)
        return glm::vec3(0);
    HitRecord rec;

    if(world.Hit(r, 0.001f, std::numeric_limits<float>::infinity(), rec)) [[likely]]
    {
        // Hemispherical scattering
        // glm::vec3 v = math::RandomInUnitSphere<float>();
        // glm::vec3 target = rec.point + (math::dot(v, rec.normal) > 0.0f ? v :
        // -v);

        // Lambertian scattering
        // glm::vec3 target = rec.point + rec.normal +
        // math::RandomOnUnitSphere<float>();

        Ray scattered;
        glm::vec3 attenuation;
        glm::vec3 emitted = rec.material->Emitted(rec.uv, rec.point);
        if(rec.material->Scatter(r, rec, attenuation, scattered)) [[likely]]
            return emitted + attenuation * RayColor(scattered, background, world, depth - 1);
        else
            return emitted;

        // return 0.5f * RayColor(Ray(rec.point, target - rec.point), world, depth -
        // 1);
    }

    // background
    return background;
}

std::string GetCurrentFilename(const std::string& base,
                               const std::string& ext, int numSamples)
{
    std::filesystem::path p = std::filesystem::current_path() / ".." / "images";

    int i = 0;
    for(const auto& entry : std::filesystem::directory_iterator(p))
    {
        if(entry.path().filename().string().find(base) == 0 && entry.path().extension().string() == ext)
            i++;
    }


    return (p / (base + std::to_string(i) + "_" + std::to_string(numSamples) + ext)).string();
}

void SaveImage(std::vector<uint8_t>& pixels, int width, int height, int numSamples)
{
    std::string filename = GetCurrentFilename("image", ".png", numSamples);
    stbi_write_png(filename.c_str(), width, height, 4, pixels.data(),
                   width * 4);
    std::cout << "\nDone.\n"
              << "Wrote to: " << filename << std::endl;
}

HittableList RandomScene()
{
    // Materials
    auto* sphereMat = g_materialAllocator.Allocate<Lambertian>(glm::vec3(0.8f, 0.f, 0.2f));
    auto* groundMat = g_materialAllocator.Allocate<Lambertian>(
        g_shapeAllocator.Allocate<CheckerTexture>(glm::vec3(0.f), glm::vec3(1.f)));
    auto* sphereLeft = g_materialAllocator.Allocate<Dielectric>(1.5f);
    auto* metalRight = g_materialAllocator.Allocate<Metal>(glm::vec3(0.8f, 0.6f, 0.2f), 1.f);
    // world
    HittableList world;

    constexpr int gridSize = 11;
    for(int a = -gridSize; a < gridSize; a++)
    {
        for(int b = -gridSize; b < gridSize; b++)
        {
            auto matChoice = math::RandomReal<float>();
            glm::vec3 center(a + 0.9f * math::RandomReal<float>(), 0.2f,
                             b + 0.9f * math::RandomReal<float>());
            if(glm::length2(center - glm::vec3(4.f, 0.2f, 0.f)) > 0.9f * 0.9f)
            {
                Material* mat;
                if(matChoice < 0.6f)
                {
                    // diffuse
                    auto albedo = math::RandomInUnitSphere<float>() * math::RandomInUnitSphere<float>();  // random color
                    mat         = g_materialAllocator.Allocate<Lambertian>(albedo);
                }
                else if(matChoice < 0.85f)
                {
                    // metal
                    auto albedo = glm::vec3(math::RandomReal<float>(0.5f, 1.0f),
                                            math::RandomReal<float>(0.5f, 1.0f),
                                            math::RandomReal<float>(0.5f, 1.0f));
                    auto fuzz   = math::RandomReal<float>(0.f, 0.5f);
                    mat         = g_materialAllocator.Allocate<Metal>(albedo, fuzz);
                }
                else
                {
                    // glass
                    mat = g_materialAllocator.Allocate<Dielectric>(1.5f);
                }
                world.Add(g_shapeAllocator.Allocate<Sphere>(center, 0.2f, mat));
            }
        }
    }
    auto* mat1 = g_materialAllocator.Allocate<Dielectric>(1.5f);
    world.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(0, 1, 0), 1.f, mat1));

    auto* mat2 = g_materialAllocator.Allocate<Lambertian>(glm::vec3(0.4f, 0.2f, 0.1f));
    world.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(-4, 1, 0), 1.f, mat2));

    auto* mat3 = g_materialAllocator.Allocate<Metal>(glm::vec3(0.7f, 0.6f, 0.5f), 0.f);
    world.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(4, 1, 0), 1.f, mat3));
    // Render into a PPM image
    //

    BVHNode worldBVH(world);

    HittableList objects;
    objects.Add(g_shapeAllocator.Allocate<BVHNode>(worldBVH));
    objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(0, -1000, 0), 1000.f,
                                                  groundMat));  // "ground"

    return objects;
}

HittableList Earth()
{
    auto* texture = g_materialAllocator.Allocate<ImageTexture>("../earthmap.jpg");
    auto* mat     = g_materialAllocator.Allocate<Lambertian>(texture);
    auto* globe   = g_shapeAllocator.Allocate<Sphere>(glm::vec3(0), 2.f, mat);

    HittableList objects;
    objects.Add(globe);
    return objects;
}
HittableList EmissionScene()
{
    auto* texture = g_materialAllocator.Allocate<ImageTexture>("earthmap.jpg");
    auto* mat1    = g_materialAllocator.Allocate<Lambertian>(texture);
    auto* mat2    = g_materialAllocator.Allocate<Emissive>(4.0f * glm::vec3(1, 1, 1));

    auto* sphere1 = g_shapeAllocator.Allocate<Sphere>(glm::vec3(0, 2, 0), 2.0f, mat1);
    auto* light   = g_shapeAllocator.Allocate<Quad>(glm::vec3(3, 1, -2), glm::vec3(2, 0, 0), glm::vec3(0, 2, 0), mat2);

    HittableList objects;
    objects.Add(sphere1);
    objects.Add(light);

    auto* groundMat = g_materialAllocator.Allocate<Lambertian>(
        g_shapeAllocator.Allocate<SolidColor>(glm::vec3(1, 1, 0)));
    objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(0, -1000, 0), 1000.f,
                                                  groundMat));  // "ground"
    // 10), -5, groundMat));
    return objects;
}
HittableList CornellBox()
{
    HittableList objects;

    auto* red   = g_materialAllocator.Allocate<Lambertian>(glm::vec3(.65, .05, .05));
    auto* white = g_materialAllocator.Allocate<Lambertian>(glm::vec3(.73, .73, .73));
    auto* green = g_materialAllocator.Allocate<Lambertian>(glm::vec3(.12, .45, .15));
    auto* light = g_materialAllocator.Allocate<Emissive>(70.0f * glm::vec3(1));

    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), glm::vec3(0, 0, 555), green));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(0, 555, 0), glm::vec3(0, 0, 555), red));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(555, 0, 0), glm::vec3(0, 0, 555), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(555, 555, 555), glm::vec3(-555, 0, 0), glm::vec3(0, 0, -555), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 555), glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(113, 554, 127), glm::vec3(330, 0, 0), glm::vec3(0, 0, 305), light));
    // objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), white));

    Hittable* box1 = g_shapeAllocator.Allocate<Box>(glm::vec3(0, 0, 0), glm::vec3(165, 330, 165), white);
    box1           = g_shapeAllocator.Allocate<RotateY>(box1, 15);
    box1           = g_shapeAllocator.Allocate<Translate>(box1, glm::vec3(265, 0, 295));
    objects.Add(box1);

    Hittable* box2 = g_shapeAllocator.Allocate<Box>(glm::vec3(0, 0, 0), glm::vec3(165, 165, 165), white);
    box2           = g_shapeAllocator.Allocate<RotateY>(box2, -18);
    box2           = g_shapeAllocator.Allocate<Translate>(box2, glm::vec3(130, 0, 65));
    objects.Add(box2);
    // objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(190, 90, 400), 90.f,
    //                                               g_materialAllocator.Allocate<Dielectric>(1.5f)));
    // objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(400, 90, 300), 85.f,
    //                                               g_materialAllocator.Allocate<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.1f)));

    return objects;
}

HittableList SmokeCornellBox()
{
    HittableList objects;

    auto* red   = g_materialAllocator.Allocate<Lambertian>(glm::vec3(.65, .05, .05));
    auto* white = g_materialAllocator.Allocate<Lambertian>(glm::vec3(.73, .73, .73));
    auto* green = g_materialAllocator.Allocate<Lambertian>(glm::vec3(.12, .45, .15));
    auto* light = g_materialAllocator.Allocate<Emissive>(7.0f * glm::vec3(1));

    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), glm::vec3(0, 0, 555), green));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(0, 555, 0), glm::vec3(0, 0, 555), red));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(555, 0, 0), glm::vec3(0, 0, 555), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(555, 555, 555), glm::vec3(-555, 0, 0), glm::vec3(0, 0, -555), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 555), glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(113, 554, 127), glm::vec3(330, 0, 0), glm::vec3(0, 0, 305), light));
    // objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), white));

    Hittable* box1 = g_shapeAllocator.Allocate<Box>(glm::vec3(0, 0, 0), glm::vec3(165, 330, 165), white);
    box1           = g_shapeAllocator.Allocate<RotateY>(box1, 15);
    box1           = g_shapeAllocator.Allocate<Translate>(box1, glm::vec3(265, 0, 295));
    objects.Add(g_shapeAllocator.Allocate<ConstantMedium>(box1, 0.002f, glm::vec3(0, 0, 0)));

    Hittable* box2 = g_shapeAllocator.Allocate<Box>(glm::vec3(0, 0, 0), glm::vec3(165, 165, 165), white);
    box2           = g_shapeAllocator.Allocate<RotateY>(box2, -18);
    box2           = g_shapeAllocator.Allocate<Translate>(box2, glm::vec3(130, 0, 65));
    objects.Add(g_shapeAllocator.Allocate<ConstantMedium>(box2, 0.002f, glm::vec3(1, 1, 1)));
    // objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(190, 90, 400), 90.f,
    //                                               g_materialAllocator.Allocate<Dielectric>(1.5f)));
    // objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(400, 90, 300), 85.f,
    //                                               g_materialAllocator.Allocate<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.1f)));

    return objects;
}
HittableList Perlin()
{
    auto* texture = g_materialAllocator.Allocate<PerlinNoise>(6);
    auto* mat     = g_materialAllocator.Allocate<Lambertian>(texture);
    auto* globe   = g_shapeAllocator.Allocate<Sphere>(glm::vec3(0), 2.f, mat);

    HittableList objects;
    objects.Add(globe);
    return objects;
}


HittableList FinalScene()
{
    HittableList boxes1;
    auto* ground = g_materialAllocator.Allocate<Lambertian>(glm::vec3(0.48, 0.83, 0.53));

    constexpr int boxesPerSide = 20;
    for(int i = 0; i < boxesPerSide; ++i)
    {
        for(int j = 0; j < boxesPerSide; ++j)
        {
            float w  = 100.0f;
            float x0 = -1000.0f + i * w;
            float z0 = -1000.0f + j * w;
            float y0 = 0.0f;
            float x1 = x0 + w;
            float y1 = math::RandomReal<float>(1, 101);
            float z1 = z0 + w;

            boxes1.Add(g_shapeAllocator.Allocate<Box>(glm::vec3(x0, y0, z0), glm::vec3(x1, y1, z1), ground));
        }
    }

    HittableList objects;
    objects.Add(g_shapeAllocator.Allocate<BVHNode>(boxes1));

    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(123, 554, 147), glm::vec3(300, 0, 0), glm::vec3(0, 0, 265),
                                                g_materialAllocator.Allocate<Emissive>(glm::vec3(7.0f))));

    objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(260, 150, 45), 50, g_materialAllocator.Allocate<Dielectric>(1.5f)));
    objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(0, 150, 145), 50, g_materialAllocator.Allocate<Metal>(glm::vec3(0.8, 0.8, 0.9), 1.0)));

    auto* boundary = g_shapeAllocator.Allocate<Sphere>(glm::vec3(360, 150, 145), 70, g_materialAllocator.Allocate<Dielectric>(1.5f));
    objects.Add(boundary);
    objects.Add(g_shapeAllocator.Allocate<ConstantMedium>(boundary, 0.2f, glm::vec3(0.2, 0.4, 0.9)));
    auto* boundary2 = g_shapeAllocator.Allocate<Sphere>(glm::vec3(0), 5000, g_materialAllocator.Allocate<Dielectric>(1.5f));
    objects.Add(g_shapeAllocator.Allocate<ConstantMedium>(boundary2, 0.0001f, glm::vec3(1)));

    auto* emat = g_materialAllocator.Allocate<Lambertian>(g_materialAllocator.Allocate<ImageTexture>("../earthmap.jpg"));
    objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(400, 200, 400), 100, emat));

    auto* pertext = g_materialAllocator.Allocate<PerlinNoise>(2);
    objects.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(220, 280, 300), 80, g_materialAllocator.Allocate<Lambertian>(pertext)));

    HittableList boxes2;
    auto* white = g_materialAllocator.Allocate<Lambertian>(glm::vec3(0.73, 0.73, 0.73));
    int ns      = 1000;
    for(int j = 0; j < ns; j++)
    {
        float x = math::RandomReal<float>(0, 165);
        float y = math::RandomReal<float>(0, 165);
        float z = math::RandomReal<float>(0, 165);
        boxes2.Add(g_shapeAllocator.Allocate<Sphere>(glm::vec3(x, y, z), 10, white));
    }

    objects.Add(g_shapeAllocator.Allocate<Translate>(g_shapeAllocator.Allocate<RotateY>(g_shapeAllocator.Allocate<BVHNode>(boxes2), 15), glm::vec3(-100, 270, 395)));

    return objects;
}

int main()
{
    // Scene
    HittableList world;
    glm::vec3 camPos;
    glm::vec3 lookAt;
    float vFOV;
    float focusDist;
    float aperture;
    glm::vec3 background;

    switch(7)
    {
    case 1:
        world      = RandomScene();
        camPos     = glm::vec3(13, 2, 3);
        lookAt     = glm::vec3(0, 0, 0);
        vFOV       = 20.f;
        focusDist  = 10.f;
        aperture   = 0.0f;
        background = glm::vec3(0.7f, 0.8f, 1.0f);
        break;
    case 2:
        world      = Earth();
        camPos     = glm::vec3(0, 2, 20);
        lookAt     = glm::vec3(0, 0, 0);
        vFOV       = 20.f;
        focusDist  = 10.f;
        aperture   = 0.0f;
        background = glm::vec3(0.7f, 0.8f, 1.0f);
        break;
    case 3:
        world      = EmissionScene();
        camPos     = glm::vec3(26, 3, 6);
        lookAt     = glm::vec3(0, 2, 0);
        vFOV       = 20.f;
        focusDist  = 10.f;
        aperture   = 0.0f;
        background = glm::vec3(0.01f);
        break;
    case 4:
        world      = CornellBox();
        camPos     = glm::vec3(278, 278, -800);
        lookAt     = glm::vec3(278, 278, 1);
        vFOV       = 40.f;
        focusDist  = 10.f;
        aperture   = 0.0f;
        background = glm::vec3(0.00f);
        break;
    case 5:
        world      = Perlin();
        camPos     = glm::vec3(0, 2, 20);
        lookAt     = glm::vec3(0, 0, 0);
        vFOV       = 20.f;
        focusDist  = 10.f;
        aperture   = 0.0f;
        background = glm::vec3(0.7f, 0.8f, 1.0f);
        break;
    case 6:
        world      = SmokeCornellBox();
        camPos     = glm::vec3(278, 278, -800);
        lookAt     = glm::vec3(278, 278, 1);
        vFOV       = 40.f;
        focusDist  = 10.f;
        aperture   = 0.0f;
        background = glm::vec3(0.00f);
        break;
    case 7:
        world      = FinalScene();
        camPos     = glm::vec3(478, 278, -600);
        lookAt     = glm::vec3(278, 278, 0);
        vFOV       = 40.f;
        focusDist  = 10.f;
        aperture   = 0.0f;
        background = glm::vec3(0.f);
        break;
    }
    constexpr uint32_t numSamples = 1;
    constexpr int maxDepth        = 50;
    constexpr float aspectRatio   = 1.f;
    16.0f / 9.0f;

    constexpr uint32_t imageHeight = 600;
    constexpr uint32_t imageWidth  = static_cast<uint32_t>(imageHeight * aspectRatio);

    // Camera
    Camera cam(camPos, lookAt, glm::vec3(0, 1, 0), vFOV, aspectRatio, aperture,
               focusDist);

    std::vector<uint32_t> imageData(imageWidth * imageHeight);
    // stb expects the bytes to be in the other order as minifb, so need to keep a separate buffer
    std::vector<uint8_t> stbImageData(imageWidth * imageHeight * 4);
    std::vector<glm::vec3> accumulatedColor(imageWidth * imageHeight, glm::vec3(0));
    int index = 0;

    std::vector<int> lines;
    lines.reserve(imageHeight);
    for(int i = imageHeight - 1; i >= 0; --i)
        lines.push_back(i);

    mfb_window* window = mfb_open("Raytracer", imageWidth, imageHeight);
    mfb_set_target_fps(60);
    if(!window)
        return 0;
    mfb_set_target_fps(60);
    uint32_t frameIndex = 0;


    auto* timer     = mfb_timer_create();
    float timer_res = 1000.0f;
    int state       = 0;
    mfb_set_keyboard_callback([&](mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed)
                              {
                                    if(key == KB_KEY_ESCAPE && isPressed)
                                        mfb_close(window);
                                    if(key == KB_KEY_S && isPressed)
                                    {
                                        SaveImage(stbImageData, imageWidth, imageHeight, frameIndex * numSamples);
                                    } },
                              window);


    do
    {
        frameIndex++;
        mfb_timer_now(timer);
        std::for_each(std::execution::par, lines.begin(), lines.end(),
                      [=, &imageData, &accumulatedColor, &stbImageData](int y)
                      {
                          for(int x = 0; x < imageWidth; ++x)
                          {
                              glm::vec3 color(0);
                              for(int s = 0; s < numSamples; ++s)
                              {
                                  float u = (x + math::RandomReal<float>()) / (imageWidth - 1);
                                  float v = (y + math::RandomReal<float>()) / (imageHeight - 1);

                                  color += RayColor(cam.GetRay(u, v), background, world, maxDepth);
                              }
                              float r = glm::max(color.r, 0.0f);
                              float g = glm::max(color.g, 0.0f);
                              float b = glm::max(color.b, 0.0f);


                              // gamma correct for gamma = 2
                              constexpr float scale = 1.0f / numSamples;

                              r = r * scale;
                              g = g * scale;
                              b = b * scale;

                              y            = imageHeight - 1 - y;
                              auto& pixel  = accumulatedColor[y * imageWidth + x];
                              pixel.r     += r;
                              pixel.g     += g;
                              pixel.b     += b;

                              glm::vec3 gammaCorrectedColor(glm::sqrt(pixel / (float)frameIndex));

                              uint8_t image_r = 256 * glm::clamp(gammaCorrectedColor.r, 0.0f, 0.999f);
                              uint8_t image_g = 256 * glm::clamp(gammaCorrectedColor.g, 0.0f, 0.999f);
                              uint8_t image_b = 256 * glm::clamp(gammaCorrectedColor.b, 0.0f, 0.999f);

                              imageData[y * imageWidth + x]              = MFB_ARGB(0xFF, image_r, image_g, image_b);
                              stbImageData[(y * imageWidth + x) * 4 + 0] = image_r;
                              stbImageData[(y * imageWidth + x) * 4 + 1] = image_g;
                              stbImageData[(y * imageWidth + x) * 4 + 2] = image_b;
                              stbImageData[(y * imageWidth + x) * 4 + 3] = 0xFF;

                              // WriteColor(std::cout, color, numSamples);
                          }
                      });

        state = mfb_update_ex(window, imageData.data(), imageWidth, imageHeight);

        std::cout << "Frametime: " << mfb_timer_delta(timer) * timer_res << " ms, Frame #" << frameIndex << std::endl;
        if(state < 0)
        {
            window = nullptr;
            break;
        }
    } while(mfb_wait_sync(window));
    SaveImage(stbImageData, imageWidth, imageHeight, frameIndex * numSamples);
}
