#include "Box.hpp"
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


#define SHAPE_ALLOCATOR_SIZE    1024 * 128
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

    BVHNode worldBVH(world, g_shapeAllocator);

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
    auto* light = g_materialAllocator.Allocate<Emissive>(150.0f * glm::vec3(1));

    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), glm::vec3(0, 0, 555), green));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(0, 555, 0), glm::vec3(0, 0, 555), red));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(555, 0, 0), glm::vec3(0, 0, 555), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(555, 555, 555), glm::vec3(-555, 0, 0), glm::vec3(0, 0, -555), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 555), glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), white));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(343, 554, 332), glm::vec3(-130, 0, 0), glm::vec3(0, 0, -105), light));
    objects.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(0, 0, 0), glm::vec3(555, 0, 0), glm::vec3(0, 555, 0), white));

    objects.Add(g_shapeAllocator.Allocate<Box>(glm::vec3(130, 0, 65), glm::vec3(295, 165, 230), white));
    objects.Add(g_shapeAllocator.Allocate<Box>(glm::vec3(265, 0, 295), glm::vec3(430, 330, 460), white));
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

    switch(4)
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
        camPos     = glm::vec3(278, 278, 0.1);
        lookAt     = glm::vec3(278, 278, 1);
        vFOV       = 90.f;
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
    }
    constexpr uint32_t numSamples = 1;
    constexpr int maxDepth        = 5;
    constexpr float aspectRatio   = 16.0f / 9.0f;

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

                              r = sqrt(r * scale);
                              g = sqrt(g * scale);
                              b = sqrt(b * scale);

                              y            = imageHeight - 1 - y;
                              auto& pixel  = accumulatedColor[y * imageWidth + x];
                              pixel.r     += r;
                              pixel.g     += g;
                              pixel.b     += b;


                              uint8_t image_r = 256 * glm::clamp(pixel.r / frameIndex, 0.0f, 0.999f);
                              uint8_t image_g = 256 * glm::clamp(pixel.g / frameIndex, 0.0f, 0.999f);
                              uint8_t image_b = 256 * glm::clamp(pixel.b / frameIndex, 0.0f, 0.999f);

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
