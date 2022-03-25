#include <iostream>
#include <stdint.h>

#include "3DMath/3DMath.h"
#include "Ray.h"
#include "Sphere.h"
#include "HittableList.h"

void WriteColor(std::ostream& out, math::Vec3d color)
{
	out << static_cast<uint32_t>(255.999 * color.r) << ' '
		<< static_cast<uint32_t>(255.999 * color.g) << ' '
		<< static_cast<uint32_t>(255.999 * color.b) << '\n';
}




math::Vec3d RayColor(const Ray& r, const HittableList& world)
{


	HitRecord rec;


	if(world.Hit(r, 0, std::numeric_limits<double>::infinity(), rec))
	{
		return 0.5 * (rec.normal + math::Vec3d(1));
	}

	// background
	double t = 0.5 * (math::normalize(r.GetDir()).y + 1.0);
	return (1.0 - t) * math::Vec3d(1.0) + t * math::Vec3d(0.5, 0.7, 1.0);

}
int main()
{
	const double aspectRatio = 16.0 / 9.0;
	const uint32_t imageWidth = 400;
	const uint32_t imageHeight = static_cast<uint32_t>(imageWidth / aspectRatio);


	// Camera
	double viewPortHeight = 2.0;
	double viewPortWidth = aspectRatio * viewPortHeight;
	double focalLength = 1.0; // distance between camera and projection plane

	math::Vec3d origin(0,0,0);
	math::Vec3d horizontal(viewPortWidth, 0, 0);
	math::Vec3d vertical(0, viewPortHeight, 0);
	math::Vec3d bottomLeft = origin - horizontal / 2.0 - vertical / 2.0 - math::Vec3d(0, 0, focalLength);

	// world
	HittableList world;
	world.Add(std::make_shared<Sphere>(math::Vec3d(0,0,-1), 0.5));
	world.Add(std::make_shared<Sphere>(math::Vec3d(0,-100.5,-1), 100)); // "ground"



	// Render into a PPM image
	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
	for(int i = imageHeight - 1; i >= 0; --i)
	{
		std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
		for(int j = 0; j < imageWidth; ++j)
		{
			double u = double(j) / (imageWidth - 1);
			double v = double(i) / (imageHeight - 1);

			Ray r(origin, bottomLeft + u * horizontal + v * vertical - origin);


			WriteColor(std::cout, RayColor(r, world));

		}
	}
	std::cerr << "\nDone.\n";


}
