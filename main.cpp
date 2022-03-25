#include <iostream>
#include <stdint.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "3DMath/3DMath.h"
#include "3DMath/Random.h"
#include "Ray.h"
#include "Sphere.h"
#include "HittableList.h"
#include "Camera.h"



void WriteColor(std::ostream& out, math::Vec3d color, uint32_t numSamples)
{
	double r = color.r;
	double g = color.g;
	double b = color.b;

	double scale = 1.0 / numSamples;
	r *= scale;
	g *= scale;
	b *= scale;

	out << static_cast<uint32_t>(256 * math::clamp(r, 0.0, 0.999)) << '\n'
		<< static_cast<uint32_t>(256 * math::clamp(g, 0.0, 0.999)) << '\n'
		<< static_cast<uint32_t>(256 * math::clamp(b, 0.0, 0.999)) << '\n';
}




math::Vec3d RayColor(const Ray& r, const HittableList& world, int depth)
{

	if(depth <= 0)
		return math::Vec3d(0);
	HitRecord rec;


	if(world.Hit(r, 0, std::numeric_limits<double>::infinity(), rec))
	{
		math::Vec3d target = rec.point + rec.normal + math::RandomInUnitSphere<double>();
		return 0.5 * RayColor(Ray(rec.point, target - rec.point), world, depth - 1);
	}

	// background
	double t = 0.5 * (math::normalize(r.GetDir()).y + 1.0);
	return (1.0 - t) * math::Vec3d(1.0) + t * math::Vec3d(0.5, 0.7, 1.0);

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
	// world
	HittableList world;
	world.Add(std::make_shared<Sphere>(math::Vec3d(0,0,-1), 0.5));
	world.Add(std::make_shared<Sphere>(math::Vec3d(0,-100.5,-1), 100)); // "ground"



	// Render into a PPM image
	std::vector<uint8_t> imageData(imageWidth * imageHeight * 3);
	int index = 0;
	for(int i = imageHeight - 1; i >= 0; --i)
	{
		std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
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

			double scale = 1.0 / numSamples;
			r *= scale;
			g *= scale;
			b *= scale;

			imageData[index++] = 256 * math::clamp(r, 0.0, 0.999);
			imageData[index++] = 256 * math::clamp(g, 0.0, 0.999);
			imageData[index++] = 256 * math::clamp(b, 0.0, 0.999);

			//WriteColor(std::cout, color, numSamples);

		}
	}
	stbi_write_png("image.png", imageWidth, imageHeight, 3, imageData.data(), imageWidth * 3);
	std::cerr << "\nDone.\n";


}
