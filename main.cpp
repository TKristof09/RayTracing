#include <iostream>
#include <stdint.h>

#include "3DMath/3DMath.h"
#include "Ray.h"

void WriteColor(std::ostream& out, math::Vec3d color)
{
	out << static_cast<uint32_t>(255.999 * color.r) << ' '
		<< static_cast<uint32_t>(255.999 * color.g) << ' '
		<< static_cast<uint32_t>(255.999 * color.b) << '\n';
}


// returns t such that Ray.At(t) is the closest point of intersection with the spehere
double HitSphere(const Ray& r, const math::Vec3d center, double radius)
{
	math::Vec3d oc = r.GetOrigin() - center;
	double a = math::lengthSq(r.GetDir());
	double half_b = math::dot(r.GetDir(), oc); // we can simplify the 2 in the return value calculation
	double c = math::lengthSq(oc) - radius * radius;

	double delta = half_b * half_b - a * c; // actually delta / 4 but doesn't matter
	if (delta < 0)
		return -1;
	else
		return (-half_b - sqrt(delta)) / a;
}

math::Vec3d RayColor(const Ray& r)
{
	math::Vec3d center = math::Vec3d(0,0,-1);
	double radius = 0.5;
	double t = HitSphere(r, center, radius);
	if(t > 0.0)
	{
		math::Vec3d normal = math::normalize(r.At(t) - center);
		return 0.5 * (normal + math::Vec3d(1));
	}

	// background
	t = 0.5 * (math::normalize(r.GetDir()).y + 1.0);
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


			WriteColor(std::cout, RayColor(r));

		}
	}
	std::cerr << "\nDone.\n";


}
