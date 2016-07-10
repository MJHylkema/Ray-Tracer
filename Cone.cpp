#include "Cone.h"
#include <math.h>

float Cone::intersect(Vector pos, Vector dir)
{   
	float dx = dir.x, dy = dir.y, dz = dir.z;
	float cx = center.x, cy = center.y, cz = center.z;
	float px = pos.x, py = pos.y, pz = pos.z;
	
    float X = px - cx;
    float Z = pz - cz;
    float Y = height - py + cy;
    
    float tan = (radius / height) * (radius / height);
    
    float a = (dx * dx) + (dz * dz) - (tan * (dy * dy));
    float b = (2 * X * dx) + (2 * Z * dz) + (2 * tan * Y * dy);
    float c = (X * X) + (Z * Z) - (tan * (Y * Y));
    
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
    
    if ((r > cy) and (r < cy + height)) 
		return t;
    else 
		return -1;
}

Vector Cone::normal(Vector p)
{
    float r = sqrt((p.x-center.x) * (p.x-center.x) + (p.z-center.z) * (p.z-center.z));
    Vector n = Vector (p.x-center.x, r * (radius/height), p.z-center.z);
    n.normalise();
    return n;
}
