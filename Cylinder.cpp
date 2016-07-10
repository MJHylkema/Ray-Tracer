#include "Cylinder.h"
#include <math.h>

float Cylinder::intersect(Vector pos, Vector dir)
{   
	float dx = dir.x, dy = dir.y, dz = dir.z;
	float xc = center.x, yc = center.y, zc = center.z;
	float px = pos.x, py = pos.y, pz = pos.z;
	
    float a = (dx * dx) + (dz * dz);
    float b = 2 * (dx * (px - xc) + dz * (pz - zc));
    float c = (px - xc) * (px - xc) + (pz - zc) * (pz - zc) - (radius * radius);
    
    float delta = b * b - 4 * (a * c);
    
	if(fabs(delta) < 0.001)
		return -1.0; 
    if(delta < 0.0) 
		return -1.0;
    
    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    float t;
    
    (t1 > t2) ? t = t2 : t = t1;
    
    float r = py + t * dy;
    
    if ((r >= yc) and (r <= yc + height))
		return t;
    else 
		return -1;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
Vector Cylinder::normal(Vector p)
{
    Vector n = Vector (p.x-center.x, 0 ,p.z-center.z);
    n.normalise();
    return n;
}
