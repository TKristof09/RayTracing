#include <iostream>
#include <stdint.h>
#include <filesystem>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "3DMath/3DMath.h"
#include "3DMath/Random.h"
#include "Ray.h"
#include "Sphere.h"
#include "HittableList.h"
#include "Camera.h"
#include "Material.h"
#include "BVH.h"
#include "Texture.h"


math::Vec3d RayColor(const Ray& r, const Hittable& world, int depth)
{

	if(depth <= 0)
		return math::Vec3d(0);
	HitRecord rec;


	if(world.Hit(r, 0.001, std::numeric_limits<double>::infinity(), rec))
	{
		// Hemispherical scattering
		//math::Vec3d v = math::RandomInUnitSphere<double>();
		//math::Vec3d target = rec.point + (math::dot(v, rec.normal) > 0.0 ? v : -v);

		// Lambertian scattering
		//math::Vec3d target = rec.point + rec.normal + math::RandomOnUnitSphere<double>();

		Ray scattered;
		math::Vec3d attenuation;
		if(rec.material->Scatter(r, rec, attenuation, scattered))
			return attenuation * RayColor(scattered, world, depth - 1);
		else
			return math::Vec3d(0);

		//return 0.5 * RayColor(Ray(rec.point, target - rec.point), world, depth - 1);
	}

	// background
	double t = 0.5 * (math::normalize(r.GetDir()).y + 1.0);
	return (1.0 - t) * math::Vec3d(1.0) + t * math::Vec3d(0.5, 0.7, 1.0);

}

std::string GetCurrentFilename(const std::string& base, const std::string& ext)
{
	std::filesystem::path p = std::filesystem::current_path();

	int i = 0;
	for(const auto& entry : std::filesystem::directory_iterator(p))
	{
		if(entry.path().filename().string().find(base) == 0 && entry.path().extension().string() == ext)
			i++;
	}

	return base + std::to_string(i) + ext;

}
int main()
{
	const uint32_t numSamples = 100;
	const int maxDepth = 50;
	const double aspectRatio = 3.0 / 2.0;

	math::Vec3d camPos = math::Vec3d(13, 2, 3);
	math::Vec3d lookAt = math::Vec3d(0, 0, 0);
	double focusDist = 10;
	double aperture = 0.0;

	const uint32_t imageWidth = 600;
	const uint32_t imageHeight = static_cast<uint32_t>(imageWidth / aspectRatio);


	// Camera
	Camera cam(camPos, lookAt, math::Vec3d(0, 1, 0), 20.0, aspectRatio, aperture, focusDist);

	// Materials
	auto sphereMat = std::make_shared<Lambertian>(math::Vec3d(0.8, 0, 0.2));
	auto groundMat = std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(math::Vec3d(0), math::Vec3d(1)));
	auto sphereLeft = std::make_shared<Dielectric>(1.5);
	auto metalRight = std::make_shared<Metal>(math::Vec3d(0.8, 0.6, 0.2), 1);

	// world
	HittableList world;

	for(int a = -11; a < 11; a++)
	{
		for(int b = -11; b < 11; b++)
		{
			auto matChoice = math::RandomReal<double>();
			math::Vec3d center(a + 0.9 * math::RandomReal<double>(), 0.2, b + 0.9 * math::RandomReal<double>());
			if(math::lengthSq(center - math::Vec3d(4, 0.2, 0)) > 0.9 * 0.9)
			{
				std::shared_ptr<Material> mat;
				if(matChoice < 0.8)
				{
					// diffuse
					auto albedo = math::RandomInUnitSphere<double>() * math::RandomInUnitSphere<double>(); // random color
					mat = std::make_shared<Lambertian>(albedo);
				}
				else if(matChoice < 0.95)
				{
					// metal
					auto albedo = math::Vec3d(math::RandomReal<double>(0.5, 1.0), math::RandomReal<double>(0.5, 1.0),math::RandomReal<double>(0.5, 1.0));
					auto fuzz = math::RandomReal<double>(0, 0.5);
					mat = std::make_shared<Metal>(albedo, fuzz);
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
	world.Add(std::make_shared<Sphere>(math::Vec3d(0,1,0), 1, mat1));

	auto mat2 = std::make_shared<Lambertian>(math::Vec3d(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(math::Vec3d(-4,1,0), 1, mat2));

	auto mat3 = std::make_shared<Metal>(math::Vec3d(0.7, 0.6, 0.5), 0);
	world.Add(std::make_shared<Sphere>(math::Vec3d(4,1,0), 1, mat3));
	// Render into a PPM image
	//

	BVHNode worldBVH(world);

	HittableList objects;
	objects.Add(std::make_shared<BVHNode>(worldBVH));
	objects.Add(std::make_shared<Sphere>(math::Vec3d(0,-1000,0), 1000, groundMat)); // "ground"

	std::vector<uint8_t> imageData(imageWidth * imageHeight * 3);
	int index = 0;
	for(int i = imageHeight - 1; i >= 0; --i)
	{
		std::cout << "\rScanlines remaining: " << i << ' ' << std::flush;
		for(int j = 0; j < imageWidth; ++j)
		{

			math::Vec3d color(0);
			for(int s = 0; s < numSamples; ++s)
			{
				double u = (j + math::RandomReal<double>()) / (imageWidth - 1);
				double v = (i + math::RandomReal<double>()) / (imageHeight - 1);

				color += RayColor(cam.GetRay(u,v), objects, maxDepth);

			}
			double r = color.r;
			double g = color.g;
			double b = color.b;

			// gamma correct for gamma = 2
			double scale = 1.0 / numSamples;
			r = sqrt(r * scale);
			g = sqrt(g * scale);
			b = sqrt(b * scale);

			imageData[index++] = 256 * math::clamp(r, 0.0, 0.999);
			imageData[index++] = 256 * math::clamp(g, 0.0, 0.999);
			imageData[index++] = 256 * math::clamp(b, 0.0, 0.999);

			//WriteColor(std::cout, color, numSamples);

		}
	}
	std::string filename = GetCurrentFilename("image", ".png");
	stbi_write_png(filename.c_str(), imageWidth, imageHeight, 3, imageData.data(), imageWidth * 3);
	std::cout << "\nDone.\n" << "Wrote to: " << filename << std::endl;


}
