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



math::Vec3d RayColor(const Ray& r, const HittableList& world, int depth)
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
	const int maxDepth = 10;
	const double aspectRatio = 16.0 / 9.0;
	const uint32_t imageWidth = 400;
	const uint32_t imageHeight = static_cast<uint32_t>(imageWidth / aspectRatio);


	// Camera
	Camera cam;

	// Materials
	auto sphereMat = std::make_shared<Lambertian>(math::Vec3d(0.8, 0.0, 0.0));
	auto groundMat = std::make_shared<Lambertian>(math::Vec3d(0.0, 0.8, 0.0));
	auto metalLeft = std::make_shared<Metal>(math::Vec3d(0.8, 0.8, 0.8), 0.3);
	auto metalRight = std::make_shared<Metal>(math::Vec3d(0.8, 0.6, 0.2), 1);

	// world
	HittableList world;
	world.Add(std::make_shared<Sphere>(math::Vec3d(0,0,-1), 0.5, sphereMat));
	world.Add(std::make_shared<Sphere>(math::Vec3d(-1,0,-1), 0.5, metalLeft));
	world.Add(std::make_shared<Sphere>(math::Vec3d(1,0,-1), 0.5, metalRight));
	world.Add(std::make_shared<Sphere>(math::Vec3d(0,-100.5,-1), 100, groundMat)); // "ground"



	// Render into a PPM image
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

				color += RayColor(cam.GetRay(u,v), world, maxDepth);

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
